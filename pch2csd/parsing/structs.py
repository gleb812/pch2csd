from pch2csd.parsing.data import mod_type_name


class Patch:
    __slots__ = ['ver', 'type', 'modules']

    def __init__(self):
        self.modules = []


class Module:
    __slots__ = ['type', 'type_name', 'id', 'location']

    def __init__(self, mod_type: int, id: int, location: str):
        self.type = mod_type
        self.type_name = mod_type_name[str(mod_type)]
        self.id = id
        self.location = location

    def __str__(self):
        return f'{self.type_name}:{self.id} ({self.location})'

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        return self.type == other.type \
               and self.type_name == other.type_name \
               and self.id == other.id \
               and self.location == other.location
