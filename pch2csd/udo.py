import abc
import os
import sys
from typing import TextIO, Tuple, List

from .patch import Module
from .resources import get_template_module_path, ProjectData
from .util import read_udo_template_lines


# UDO representation

class UdoAnnotation:
    prefix = ';@'

    def __init__(self, txt: str, line: int):
        toks = txt.split()
        if len(toks) > 0 and toks[0] == self.prefix:
            self.tokens = toks
        else:
            self.tokens = []
        self.line = line

    def parsed(self) -> bool:
        return len(self.tokens) > 0

    @staticmethod
    def try_to_parse(txt: str, line: int):
        a = UdoAnnotation(txt, line)
        if not a.parsed():
            return a
        if a.tokens[1] == MapAnnotation.atype:
            return MapAnnotation(txt, line)
        elif a.tokens[1] == ArgsAnnotation.atype:
            return ArgsAnnotation(txt, line)
        else:
            return a


class MapAnnotation(UdoAnnotation):
    atype = 'map'

    def __init__(self, txt: str, line: int):
        super().__init__(txt, line)
        toks = self.tokens

        if super().parsed() \
                and len(toks) >= 4 \
                and toks[1] == self.atype \
                and toks[2] in 'ds':
            self.map_type = toks[2]
            self.switch_ref = [] if self.map_type == 'd' else [toks[3]]
            self.tables = toks[3:] if self.map_type == 'd' else toks[4:]
        else:
            self.map_type = None
            self.tables = None

    def parsed(self):
        return super().parsed() \
               and self.map_type is not None \
               and self.tables is not None


class ArgsAnnotation(UdoAnnotation):
    atype = 'args'

    def __init__(self, txt: str, line: int):
        super().__init__(txt, line)
        toks = self.tokens

        if super().parsed() \
                and len(toks) > 2 \
                and toks[1] == self.atype:
            args = ''.join(toks[2:]).split(',')
            if all([a in 'ika' for arg in args for a in arg]):
                self.args = args
            else:
                self.args = None
        else:
            self.args = None

    def parsed(self):
        return super().parsed() \
               and self.args is not None \
               and len(self.args) == 3


class Opcode:
    def __init__(self, src: List[str], lines: Tuple[int, int]):
        assert len(src) == (lines[1] - lines[0] + 1)
        assert src[0].strip().startswith('opcode')
        assert src[-1].strip().startswith('endop')

        self.src = [s.strip() for s in src]
        self.lines = lines


class UdoTemplate:
    def __init__(self, mod: Module):
        self.mod_type = mod.type
        self.mod_type_name = mod.type_name
        self.lines = read_udo_template_lines(mod.type)
        # self.args_lines, self.maps_lines, self.args, self.maps = self._parse_headers()
        self.annots, self.opcodes = self._parse_template()

    @property
    def args(self):
        return [a.args for a in self.annots
                if isinstance(a, ArgsAnnotation)]

    @property
    def args_lines(self):
        return [a.line for a in self.annots
                if isinstance(a, ArgsAnnotation)]

    @property
    def maps(self):
        t = [[m.map_type] + m.switch_ref + m.tables
             for m in self.annots
             if isinstance(m, MapAnnotation)]
        return t

    @property
    def maps_lines(self):
        return [m.line for m in self.annots
                if isinstance(m, MapAnnotation)]

    def __repr__(self):
        return 'UdoTemplate({}, {}.txt)'.format(self.mod_type_name, self.mod_type)

    def __str__(self):
        return self.__repr__()

    @property
    def filename(self):
        return '{}.txt'.format(self.mod_type)

    @property
    def path(self):
        return get_template_module_path(self.mod_type)

    def _parse_template(self):
        annots = [(i, l) for i, l
                  in enumerate(self.lines)
                  if l.strip().startswith(';@')]
        annots = [UdoAnnotation.try_to_parse(txt, line)
                  for line, txt in annots]

        opcodes_lines = [i for i, l
                         in enumerate(self.lines)
                         if l.strip().startswith('opcode')
                         or l.strip().startswith('endop')]
        opcodes = [Opcode(self.lines[i1:i2 + 1], (i1, i2))
                   for i1, i2
                   in zip(*[iter(opcodes_lines)] * 2)]

        return annots, opcodes

    def validate(self, data: ProjectData):
        v = UdoTemplateValidation(data, self)
        return v.is_valid()


# Validation

class UdoValidation(metaclass=abc.ABCMeta):
    def __init__(self, data: ProjectData, tpl: UdoTemplate):
        self.messages = []
        self._validate(data, tpl)

    @property
    def is_valid(self) -> bool:
        return self.messages is not None and len(self.messages) > 0

    @abc.abstractmethod
    def _validate(self, data: ProjectData, tpl: UdoTemplate):
        raise NotImplementedError


class TemplateExists(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data, tpl):
        if not os.path.isfile(tpl.path):
            self.messages.append(
                'error: no template file for the module with id {}'.format(tpl.mod_type))


class ArgsAnnotationsValid(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data, tpl):
        if len(tpl.args) == 0:
            self.messages.append(
                "error ({}): no 'args' annotations were found "
                "in the template".format(tpl.filename))
            return
        for i, a in enumerate(tpl.args):
            if len(a) != 3:
                self.messages.append(
                    "error ({}): the 'args' annotation should have "
                    "exactly three arguments".format(tpl.filename))
        if len(tpl.args[0][0]) != len(tpl.maps):
            self.messages.append(
                "error ({}): the number of 'map' annotations "
                "should be equal "
                "to the number of module parameters".format(tpl.filename))


class MapTablesValid(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data, tpl):
        unknown_map_types = set()
        unknown_map_tables = set()
        for m in tpl.maps:
            if m[0] not in 'ds':
                unknown_map_types.add(m[0])
            offset = 2 if m[0] == 's' else 1
            for t in m[offset:]:
                if t not in data.value_maps:
                    unknown_map_tables.add(t)

        if len(unknown_map_types) > 0:
            self.messages.append(
                'error ({}): unknown mapping types: {}'.format(
                    tpl.filename,
                    ', '.join(unknown_map_types)))
        if len(unknown_map_tables) > 0:
            self.messages.append(
                'error ({}): unknown mapping tables: {}'.format(
                    tpl.filename,
                    ', '.join(unknown_map_tables)))


class ToDoCollect(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data: ProjectData, tpl: UdoTemplate):
        todos = [l[l.find(';'):].
                     replace(';', '').
                     replace(':', '').
                     replace('TODO', '').strip()
                 for l in tpl.lines
                 if 'TODO' in l]
        self.messages.extend([t for t in todos if t != ''])


class UdoTemplateValidation:
    def __init__(self, data: ProjectData, tpl: UdoTemplate):
        tpl_exists = TemplateExists(data, tpl)
        if tpl_exists.is_valid:
            validators = [ArgsAnnotationsValid,
                          MapTablesValid]
            self.errors = [msg
                           for v in validators
                           for msg in v(data, tpl).messages]
        else:
            self.errors = tpl_exists.messages
        self.todos = ToDoCollect(data, tpl).messages

    def is_valid(self, with_todos=False):
        if len(self.errors) > 0:
            return False
        if with_todos and len(self.todos) > 0:
            return False
        return True

    def print_errors(self, io: TextIO = sys.stdout):
        if len(self.errors) == 0 and len(self.todos) == 0:
            print('OK', file=io)
        if len(self.errors) > 0:
            for e in self.errors:
                print('{}'.format(e), file=io)
        if len(self.todos) > 0:
            for t in self.todos:
                print('TODO: {}'.format(t), file=io)
