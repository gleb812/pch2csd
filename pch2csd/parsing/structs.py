from enum import Enum

from pch2csd.parsing.data import mod_type_name
from pch2csd.parsing.util import AttrEqMixin


class Patch:
    __slots__ = ['ver', 'type', 'modules', 'cables', 'mod_params']

    def __init__(self):
        self.modules = []
        self.cables = []
        self.mod_params = []


class Location(Enum):
    FX_AREA = 0,
    VOICE_AREA = 1

    @staticmethod
    def from_int(i: int):
        if i == 0:
            return Location.FX_AREA
        elif i == 1:
            return Location.VOICE_AREA
        else:
            raise ValueError(f'Wrong location code: {i}')


class Module:
    __slots__ = ['location', 'type', 'type_name', 'id']

    def __init__(self, loc: Location, mod_type: int, id: int):
        self.type = mod_type
        self.type_name = mod_type_name[str(mod_type)]
        self.id = id
        self.location = loc

    def __str__(self):
        return f'{self.type_name}:{self.id} ({self.location})'

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        return self.type == other.type \
               and self.type_name == other.type_name \
               and self.id == other.id \
               and self.location == other.location


class CableType(Enum):
    IN_TO_IN = 0
    OUT_TO_IN = 1

    @staticmethod
    def from_int(i: int):
        if i == 0:
            return CableType.IN_TO_IN
        elif i == 1:
            return CableType.OUT_TO_IN
        else:
            raise ValueError(f'Wrong cable type code: {i}')


class CableColor(Enum):
    RED = 0
    BLUE = 1
    YELLOW = 2
    ORANGE = 3
    GREEN = 4
    PURPLE = 5
    WHITE = 6

    @staticmethod
    def from_int(i: int):
        if i == 0:
            return CableColor.RED
        elif i == 1:
            return CableColor.BLUE
        elif i == 2:
            return CableColor.YELLOW
        elif i == 3:
            return CableColor.ORANGE
        elif i == 4:
            return CableColor.GREEN
        elif i == 5:
            return CableColor.PURPLE
        elif i == 6:
            return CableColor.WHITE
        else:
            raise ValueError(f'Wrong cable color code {i}')


class Cable:
    __slots__ = ['loc', 'type', 'color', 'module_from', 'jack_from', 'module_to', 'jack_to']

    def __init__(self, loc: Location, type: CableType, color: CableColor, module_from: int,
                 jack_from: int, module_to: int, jack_to: int):
        self.loc = loc
        self.type = type
        self.color = color
        self.module_from = module_from
        self.jack_from = jack_from
        self.module_to = module_to
        self.jack_to = jack_to

    def __str__(self):
        return f'Cable: {self.type, self.color}' \
               f'{self.module_from}:{self.jack_from} ' \
               f'{self.module_to}:{self.jack_to}'

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        return self.loc == other.loc \
               and self.type == other.type \
               and self.color == other.color \
               and self.module_from == other.module_from \
               and self.jack_from == other.jack_from \
               and self.module_to == other.module_to \
               and self.jack_to == other.jack_to


class ModuleParameters(AttrEqMixin):
    def __init__(self, loc: Location, module_id: int, num_params: int, values=None):
        if values is None:
            values = []
        self.loc = loc
        self.module_id = module_id
        self.num_params = num_params
        self.values = values

    def __eq__(self, other):
        return self.attrs_equal(other)
