from enum import Enum
from typing import List, Optional

from pch2csd.resources import ProjectData
from pch2csd.util import AttrEqMixin, ReprStrMixin


class Location(Enum):
    FX_AREA = 0
    VOICE_AREA = 1

    @staticmethod
    def from_int(i: int):
        if i == 0:
            return Location.FX_AREA
        elif i == 1:
            return Location.VOICE_AREA
        else:
            raise ValueError('Wrong location code: {}'.format(i))

    @staticmethod
    def from_str(s: str):
        s = s.lower()
        if s == 'voice':
            return Location.VOICE_AREA
        elif s == 'fx':
            return Location.FX_AREA
        else:
            raise ValueError('Wrong location string: {}'.format(s))

    def short_str(self):
        if self.value == 0:
            return 'FX'
        elif self.value == 1:
            return 'VOICE'


class PatchDescription(ReprStrMixin):
    def __init__(self):
        self.active_variation = 0  # TODO support variations


class Module(AttrEqMixin, ReprStrMixin):
    def __init__(self, data: ProjectData, loc: Location, mod_type: int, id: int, modes=None):
        if modes is None:
            modes = []
        self.type = mod_type
        self.type_name = data.mod_type_name[mod_type]
        self.id = id
        self.location = loc
        self.modes = modes

    def __eq__(self, other):
        return self.attrs_equal(other)


class ModuleA2K(Module):
    def __init__(self, data: ProjectData, loc: Location):
        super().__init__(data, loc, 999, data.new_mod_id)

    def get_io_cable_colors(self):
        return CableColor.RED, CableColor.BLUE


class ModuleK2A(Module):
    def __init__(self, data: ProjectData, loc: Location):
        super().__init__(data, loc, 1000, data.new_mod_id)

    def get_io_cable_colors(self):
        return CableColor.BLUE, CableColor.RED


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
            raise ValueError('Wrong cable type code: {}'.format(i))

    def short_str(self):
        if self.value == 0:
            return 'in-in'
        elif self.value == 1:
            return 'out-in'


class CableColor(Enum):
    RED = 0
    BLUE = 1
    YELLOW = 2
    ORANGE = 3
    GREEN = 4
    PURPLE = 5
    WHITE = 6

    def short_str(self):
        colors = ['red', 'blue', 'yellow', 'orange', 'green', 'purple', 'white']
        return colors[self.value]

    @staticmethod
    def from_int(i: int):
        colors = [CableColor.RED, CableColor.BLUE, CableColor.YELLOW, CableColor.ORANGE,
                  CableColor.GREEN,
                  CableColor.PURPLE, CableColor.WHITE]
        if 0 <= i < 7:
            return colors[i]
        else:
            raise ValueError('Wrong cable color code {}'.format(i))

    @staticmethod
    def to_cs_rate_char(c):
        if c == CableColor.RED:
            return 'a'
        elif c == CableColor.BLUE:
            return 'k'
        elif c == CableColor.YELLOW:
            return 'k'
        elif c == CableColor.ORANGE:
            return 'a'
        elif c == CableColor.GREEN:
            raise NotImplementedError
        elif c == CableColor.PURPLE:
            raise NotImplementedError
        elif c == CableColor.WHITE:
            raise NotImplementedError
        else:
            raise ValueError('Unknown cable color: {}'.format(c))


class Cable(AttrEqMixin, ReprStrMixin):
    def __init__(self, loc: Location, type: CableType, color: CableColor, module_from: int,
                 jack_from: int, module_to: int, jack_to: int):
        self.loc = loc
        self.type = type
        self.color = color
        self.module_from = module_from
        self.jack_from = jack_from
        self.module_to = module_to
        self.jack_to = jack_to

    def __eq__(self, other):
        return self.attrs_equal(other)


class ModuleParameters(AttrEqMixin, ReprStrMixin):
    def __init__(self, loc: Location, module_id: int, num_params: int, values=None, modes=None):
        if values is None:
            values = []
        self.loc = loc
        self.module_id = module_id
        self.num_params = num_params
        self.values = values

    def __eq__(self, other):
        return self.attrs_equal(other)


class Patch(ReprStrMixin):
    __slots__ = ['data', 'ver', 'type', 'modules', 'cables', 'mod_params']

    def __init__(self, data: ProjectData):
        self.data = data
        self.description = PatchDescription()
        self.modules = []  # List[Module]
        self.cables = []  # List[Cable]
        self.mod_params = []  # List[ModuleParameters]

    def find_module(self, id: int, loc=Location.VOICE_AREA) -> Optional[Module]:
        for m in self.modules:
            if m.id == id and m.location == loc:
                return m
        return None

    def find_incoming_cable(self, loc: Location,
                            mod_to: int, jack_to: int = None) -> Optional[Cable]:
        for c in self.cables:
            if c.module_to == mod_to and c.jack_to == jack_to and c.loc == loc:
                return c
        return None

    def find_all_incoming_cables(self, loc: Location, mod_to: int) -> Optional[List[Cable]]:
        cables = [c for c in self.cables if c.loc == loc and c.module_to == mod_to]
        return cables if len(cables) > 0 else None

    def find_all_outgoing_cables(self, loc: Location, mod_from: int) -> Optional[List[Cable]]:
        cables = [c for c in self.cables if c.loc == loc and c.module_from == mod_from]
        return cables if len(cables) > 0 else None

    def find_mod_params(self, loc: Location, mod_id: int) -> ModuleParameters:
        for p in self.mod_params:
            if p.loc == loc and p.module_id == mod_id:
                return p
        return ModuleParameters(loc, mod_id, 0, [])


def transform_in2in_cables(patch: Patch, cable: Cable) -> Optional[Cable]:
    if cable.type == CableType.OUT_TO_IN:
        return cable
    c = cable
    while c is not None:
        if c.type == CableType.OUT_TO_IN:
            break
        c = patch.find_incoming_cable(c.loc, c.module_from, c.jack_from)
    if c is not None:
        return Cable(c.loc, c.type, c.color, c.module_from, c.jack_from, cable.module_to,
                     cable.jack_to)
    else:
        return None
