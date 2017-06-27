from typing import List

from pch2csd.csdgen.util import LogMixin
from pch2csd.data import ProjectData, get_template_module_path
from pch2csd.parsing.structs import Module, Patch


class UdoTemplate(LogMixin):
    def __init__(self, mod: Module):
        self._repr = f'{mod.type_name}({mod.type})'
        self.logger_set_name('UdoTemplate')
        with open(get_template_module_path(mod.type)) as f:
            self.tpl_lines = [l.strip() for l in f]
        self.headers = self._parse_headers()

    def __repr__(self):
        return f'UdoTemplate({self._repr})'

    def _parse_headers(self) -> List[List[str]]:
        headers: List[str] = []
        for l in self.tpl_lines:
            if l.startswith(';@'):
                headers.append([s.strip() for s in l.replace(';@', '').split(',')])
        if len(headers) == 0:
            self.log.error('%s: no opcode headers were found in the template', self._repr)
            return []
        for h in headers:
            if len(h) != 3:
                self.log.error('%s: opcode header should have exactly three arguments', self._repr)
                return []
        return headers


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
