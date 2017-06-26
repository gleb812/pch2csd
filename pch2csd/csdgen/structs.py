from pch2csd.data import ProjectData
from pch2csd.parsing.structs import Module, Patch


class UdoTemplate:
    def __init__(self, mod_type: int):
        pass


class CsModule:
    def __init__(self, data: ProjectData, patch: Patch, mod: Module):
        self.mod = mod
        self.params = data.mod_params[self.mod.type]
        io = data.mod_type_connections[self.mod.type]
        self.inlet_types = io['inputs']
        self.outlet_types = io['outputs']
        # Default buses
        self.inlet_conn = [0 for _ in self.inlet_types]
        self.outlet_conn = [1 for _ in self.outlet_types]
