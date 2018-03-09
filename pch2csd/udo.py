import abc
import os
import sys
from typing import TextIO

from .patch import Module
from .resources import get_template_module_path, ProjectData
from tests.util import clean_up_string


class UdoTemplate:
    def __init__(self, mod: Module):
        self.mod_type = mod.type
        self.mod_type_name = mod.type_name
        try:
            with open(get_template_module_path(mod.type), 'r') as f:
                self.lines = [l.strip() for l in f]
        except IOError:
            self.lines = []
        self.args_lines, self.maps_lines, self.args, self.maps = self._parse_headers()

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

    def _parse_headers(self):
        args_lines = []
        maps_lines = []
        args = []  # List[List[str]]
        maps = []  # List[List[str]]

        for i, line in enumerate(self.lines):
            l = clean_up_string(line)
            if l.startswith(';@ args'):
                args_lines.append(i)
                a = [s.strip() for s in l.replace(';@ args', '').split(',')]
                # for those who are used to put 0 to declare noargs
                a = [x if x != '0' else '' for x in a]
                args.append(a)
            elif l.startswith(';@ map'):
                maps_lines.append(i)
                m = [s.strip() for s in l.replace(';@ map', '').strip().split(' ')]
                maps.append(m)
        return args_lines, maps_lines, args, maps

    def validate(self, data: ProjectData):
        v = UdoTemplateValidation(data, self)
        return v.is_valid()


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
        if tpl.mod_type == 199:
            print('lal')
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
            print('This template appears to be OK', file=io)
        if len(self.errors) > 0:
            for e in self.errors:
                print('{}'.format(e), file=io)
        if len(self.todos) > 0:
            for t in self.todos:
                print('TODO: {}'.format(t), file=io)
