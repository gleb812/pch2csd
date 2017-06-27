from typing import List

from pch2csd.csdgen.util import LogMixin
from pch2csd.data import get_template_module_path
from pch2csd.parsing.structs import Module, Patch, CableColor


class UdoTemplate(LogMixin):
    def __init__(self, mod: Module):
        self._repr = f'{mod.type_name}({mod.type})'
        self.logger_set_name('UdoTemplate')
        with open(get_template_module_path(mod.type)) as f:
            self.lines = [l.strip() for l in f]
        self.headers = self._parse_headers()

    def __repr__(self):
        return f'UdoTemplate({self._repr})'

    def _parse_headers(self) -> List[List[str]]:
        headers: List[str] = []
        for l in self.lines:
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


class UdoStatement:
    def __init__(self, patch: Patch, mod: Module):
        self.patch = patch
        self.mod = mod
        self.tpl = UdoTemplate(mod)
        self.udo_variant = self._choose_udo_variant()

    def __repr__(self):
        return self.get_udo_name()

    def get_udo_name(self) -> str:
        if len(self.tpl.headers) < 2:
            return self.mod.type_name
        else:
            return f'{self.mod.type_name}_v{self.udo_variant}'

    def get_udo_code(self) -> str:
        if len(self.tpl.headers) < 2:
            return '\n'.join(self.tpl.lines)
        v = self.udo_variant
        lines = self.tpl.lines
        h_pos = [i for i, l in enumerate(lines) if l.startswith(';@')] + [len(lines)]
        return '\n'.join(lines[h_pos[v]:h_pos[v + 1]])

    def _choose_udo_variant(self) -> int:
        v = 0
        cables = self.patch.find_all_incoming_cables(self.mod.location, self.mod.id)
        if len(self.tpl.headers) < 2 or cables is None:
            return v
        cs_rates = {c.jack_to: CableColor.to_cs_rate_char(c.color) for i, c in enumerate(cables)}
        tpl_v0_ins = self.tpl.headers[0][1]
        tpl_v1_ins = self.tpl.headers[1][1]
        for i, r in cs_rates.items():
            if tpl_v0_ins[i] != r and tpl_v1_ins[i] == r:
                v = 1
                break
        return v
