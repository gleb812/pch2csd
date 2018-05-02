import abc
import os
import sys
from typing import TextIO, Tuple, List

from .patch import Module, Patch, ModuleParameters
from .resources import get_template_module_path, ProjectData
from .util import read_udo_template_lines
from tests import get_all_module_pch2


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
        elif a.tokens[1] == ModeAnnotation.atype:
            return ModeAnnotation(txt, line)
        elif a.tokens[1] == InsAnnotation.atype:
            return InsAnnotation(txt, line)
        elif a.tokens[1] == OutsAnnotation.atype:
            return OutsAnnotation(txt, line)
        else:
            return a


class MapAnnotation(UdoAnnotation):
    atype = 'map'

    def __init__(self, txt: str, line: int):
        super().__init__(txt, line)

        self.name = None
        self.tables = None

        toks = self.tokens

        if super().parsed() \
                and len(toks) >= 3 \
                and toks[1] == self.atype:
            self.name = toks[2]
            self.tables = [t for t in toks[2:] if t not in 'ds0123456789']

    def parsed(self):
        return super().parsed() \
               and self.tables is not None


class ModeAnnotation(UdoAnnotation):
    atype = 'mode'

    def __init__(self, txt: str, line: int):
        super().__init__(txt, line)

        self.name = None
        self.labels = None

        toks = self.tokens

        if super().parsed() \
                and len(toks) >= 3 \
                and toks[1] == self.atype:
            self.name = toks[2]
            self.labels = toks[2:]


class InsAnnotation(UdoAnnotation):
    atype = 'ins'

    def __init__(self, txt: str, line: int, artificial=False):
        super().__init__(txt, line)
        toks = self.tokens

        self.types = []
        self.names = []
        self.artificial = artificial

        if super().parsed() \
                and len(toks) > 2 \
                and toks[1] == self.atype:
            args = [(b[0], b[1]) if len(b) > 1 else (b[0], None)
                    for b in [a.split(':') for a in toks[2:]]]
            self.types, self.names = list(zip(*args))

    def parsed(self):
        if self.artificial:
            return True
        else:
            return super().parsed() \
                   and self.types != [] \
                   and self.names != []


class OutsAnnotation(InsAnnotation):
    atype = 'outs'

    def __init__(self, txt: str, line: int, artificial=False):
        super().__init__(txt, line, artificial)


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
        self.annots, self.opcodes = self._parse_template()
        if len(self.ins) == 0:
            self.annots += [InsAnnotation('', -1, artificial=True)
                            for o in self.opcodes]
        if len(self.outs) == 0:
            self.annots += [OutsAnnotation('', -1, artificial=True)
                            for o in self.opcodes]

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

    def _get_annotation(self, annotType):
        return [a for a in self.annots if type(a) == annotType]

    @property
    def maps(self):
        return self._get_annotation(MapAnnotation)

    @property
    def modes(self):
        return self._get_annotation(ModeAnnotation)

    @property
    def ins(self):
        return self._get_annotation(InsAnnotation)

    @property
    def outs(self):
        return self._get_annotation(OutsAnnotation)

    def _parse_template(self):
        annots = [(i, l) for i, l
                  in enumerate(self.lines)
                  if l.strip().startswith(';@')]
        annots = [UdoAnnotation.try_to_parse(txt, line)
                  for line, txt in annots]
        for i, a in enumerate(annots):
            if type(a) == MapAnnotation:
                a.idx = i

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
        self.could_not_validate = False
        self._validate(data, tpl)

    @property
    def is_valid(self) -> bool:
        return self.messages is not None \
               and not self.could_not_validate \
               and len(self.messages) == 0

    @abc.abstractmethod
    def _validate(self, data: ProjectData, tpl: UdoTemplate):
        raise NotImplementedError


class ValidationChain:
    def __init__(self, validator_classes: list):
        self.validators = validator_classes
        self.errors = []

    def do(self, data: ProjectData, tpl: UdoTemplate):
        for validator in self.validators:
            v = validator(data, tpl)
            if not v.is_valid:
                self.errors += v.messages
                break


class TemplateExists(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data, tpl):
        if not os.path.isfile(tpl.path):
            self.messages.append(
                'error: no template file for the module with id {}'.format(tpl.mod_type))


class AnnotationParsed(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data: ProjectData, tpl: UdoTemplate):
        not_parsed = [a for a in tpl.annots if not a.parsed()]
        msg = "error ({}, line {}): can't parse the annotation"
        self.messages = [msg.format(tpl.filename, a.line + 1)
                         for a in not_parsed]


class MapTablesExist(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data: ProjectData, tpl: UdoTemplate):
        invalid_tables = [t for a in tpl.annots
                          if isinstance(a, MapAnnotation)
                          if a.parsed()
                          for t in a.tables
                          if t not in data.value_maps.keys()]

        msg = 'error ({}): non-existent tables are used in the ' \
              'map annotation(s): {}'
        if len(invalid_tables) > 0:
            self.messages = [msg.format(tpl.filename, ', '.join(invalid_tables))]


class UdoIntypesConsistent(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data: ProjectData, tpl: UdoTemplate):
        try:
            in_udo = [o.src[0].split(',')[2].strip() for o in tpl.opcodes]
        except IndexError:
            self.messages += ["error ({}): can't read one or more " \
                              "UDO signatures".format(tpl.filename)]
            return

        in_len = [len(i) for i in in_udo]
        if len(set(in_len)) != 1:
            self.messages += ["error ({}): UDO variants have different "
                              "number of intypes".format(tpl.filename)]
        else:
            intypes_atleast = len(tpl.modes) \
                              + len(tpl.ins[0].types) \
                              + len(tpl.outs[0].types)
            if in_len[0] < intypes_atleast:
                self.messages += ["error ({}): according to annotations "
                                  "there should be at least {} 'intypes' "
                                  "in the UDO".format(tpl.filename, intypes_atleast)]


class ParamsModesConsistent(UdoValidation):
    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data: ProjectData, tpl: UdoTemplate):
        if not all([a.parsed() for a in tpl.maps + tpl.modes]):
            self.could_not_validate = True
            return

        msg = "error ({}): the number of {} defined in the template " \
              "does not equal to the number of {} found in a test patch"
        mod_params = {m: p.find_mod_params(m.location, m.id)
                      for p in get_all_module_pch2()
                      for m in p.modules
                      if m.type == tpl.mod_type}
        assert len(mod_params) == 1
        mod, params = list(mod_params.items())[0]
        if params.num_params != len(tpl.maps):
            self.messages += [msg.format(tpl.filename,
                                         "map annotations",
                                         "corresponding module's parameters")]
        if len(mod.modes) != len(tpl.modes):
            self.messages += [msg.format(tpl.filename,
                                         "mode annotations",
                                         "corresponding module's modes")]


class InsOutsValid(UdoValidation):
    excluded_modules = []

    def __init__(self, data, tpl):
        super().__init__(data, tpl)

    def _validate(self, data: ProjectData, tpl: UdoTemplate):
        if len(tpl.ins) == len(tpl.outs) == 0 \
                and tpl.mod_type not in self.excluded_modules:
            self.messages += ["error ({}): the UDO must have at least one "
                              "of 'ins' or 'outs' annotations".format(tpl.filename)]


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
        self.errors = self._perform_standard_validations(data, tpl)
        self.todos = ToDoCollect(data, tpl).messages

    @staticmethod
    def _perform_standard_validations(data: ProjectData, tpl: UdoTemplate) -> List[str]:
        exists = TemplateExists(data, tpl)
        if not exists.is_valid:
            return exists.messages
        validators = ValidationChain([AnnotationParsed,
                                      MapTablesExist,
                                      InsOutsValid,
                                      UdoIntypesConsistent])
        validators.do(data, tpl)
        return validators.errors

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
