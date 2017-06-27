import os
from copy import deepcopy
from glob import glob
from io import StringIO
from typing import List, Dict, Tuple

from pch2csd.data import get_template_module_path, get_template_path, get_template_dir
from pch2csd.parsing.structs import Module, Patch, CableColor, Cable
from pch2csd.util import LogMixin, preprocess_csd_code


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
        headers = []
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


class Udo(LogMixin):
    def __init__(self, patch: Patch, mod: Module):
        self.patch = patch
        self.mod = mod
        self.tpl = UdoTemplate(mod)
        self.udo_variant = self._choose_udo_variant()
        _, self.in_types, self.out_types = self.header
        self.inlets, self.outlets = self._init_zak_connections()

    def __repr__(self):
        return self.get_name()

    @property
    def header(self):
        return self.tpl.headers[self.udo_variant]

    def get_name(self) -> str:
        if len(self.tpl.headers) < 2:
            return self.mod.type_name
        else:
            return f'{self.mod.type_name}_v{self.udo_variant}'

    def get_src(self) -> str:
        if len(self.tpl.headers) < 2:
            return '\n'.join(self.tpl.lines)
        v = self.udo_variant
        lines = self.tpl.lines
        h_pos = [i for i, l in enumerate(lines) if l.startswith(';@')] + [len(lines)]
        udo_lines = lines[h_pos[v] + 1:h_pos[v + 1]]
        udo_lines[0] = udo_lines[0].replace(self.mod.type_name, self.get_name())
        return '\n'.join(udo_lines)

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

    def get_params(self) -> List[float]:
        tpl_param_def = self.tpl.headers[self.udo_variant][0]
        params = self.patch.find_mod_params(self.mod.location, self.mod.id)
        if len(tpl_param_def) != len(params.values):
            self.log.error(f"Template '{self.tpl}' has different number of parameters "
                           f"than it was found in the parsed module '{self.mod}'. "
                           "Returning -1s for now.")
            return [-1] * params.num_params
        # TODO mappings
        return params.values

    def get_statement(self):
        s = StringIO()
        s.write(self.get_name())
        s.write('(')
        s.write('/* Params */ ')
        s.write(', '.join([str(f) for f in self.get_params()]))
        s.write(', /* Inlets */ ')
        s.write(', '.join([str(i) for i in self.inlets]))
        s.write(', /* Outlets */ ')
        s.write(', '.join([str(i) for i in self.outlets]))
        s.write(')')
        return s.getvalue()

    def _init_zak_connections(self):
        ins, outs = self.in_types, self.out_types
        return [ZakSpace.trash_bus] * len(ins), [ZakSpace.zero_bus] * len(outs)


class ZakSpace:
    zero_bus = 0
    trash_bus = 1

    def __init__(self):
        self.aloc_i = 2
        self.kloc_i = 2
        self.zakk: Dict[Tuple[int, int], int] = {}  # Tuple[mod_id, inlet_id] -> zak_loc
        self.zaka: Dict[Tuple[int, int], int] = {}  # Tuple[mod_id, inlet_id] -> zak_loc

    def _init_zak(self):
        self.aloc_i = 2
        self.kloc_i = 2
        self.zakk: Dict[Tuple[int, int], int] = {}  # Tuple[mod_id, inlet_id] -> zak_loc
        self.zaka: Dict[Tuple[int, int], int] = {}  # Tuple[mod_id, inlet_id] -> zak_loc

    def connect_patch(self, p: Patch) -> List[Udo]:
        self._init_zak()
        udos: Dict[int, Udo] = deepcopy({m.id: Udo(p, m) for m in p.modules})
        for c in p.cables:
            mf, jf, mt, jt = c.module_from, c.jack_from, c.module_to, c.jack_to
            if udos[mf].out_types[jf] == udos[mt].in_types[jt]:
                self._zak_connect_direct(c, udos)
            else:
                raise NotImplementedError('Patch cord type conversion is not implemented yet.')
        return list(udos.values())

    def _aloc_new(self) -> int:
        self.aloc_i += 1
        return self.aloc_i

    def _kloc_new(self) -> int:
        self.kloc_i += 1
        return self.kloc_i

    def _zak_connect_direct(self, c: Cable, udos: Dict[int, Udo]):
        mf, jf, mt, jt = c.module_from, c.jack_from, c.module_to, c.jack_to
        rate_type = udos[mf].out_types[jf]
        if rate_type not in 'ka':
            raise ValueError(f'Unknown rate type: {rate_type}')
        out_id = (mf, jf)
        zak = self.zakk if rate_type == 'k' else self.zaka
        if out_id not in zak:
            zak[out_id] = self._kloc_new() if rate_type == 'k' else self._aloc_new()
        zak_id = zak[out_id]
        udos[mf].outlets[jf] = zak_id
        udos[mt].inlets[jt] = zak_id


class Csd:
    def __init__(self, p: Patch, zak: ZakSpace, udos: List[Udo]):
        self.patch = p
        self.zak = zak
        self.udos = udos

    def get_code(self) -> str:
        return '\n'.join([self.header,
                          self.zakinit,
                          self.ft_statements,
                          self.udo_defs,
                          self.instr_va,
                          self.instr_fx,
                          self.footer])

    @property
    def header(self) -> str:
        path = get_template_path('csd_header')
        with open(path, 'r') as f:
            return preprocess_csd_code(f.read()) + '\n'

    @property
    def footer(self) -> str:
        path = get_template_path('csd_footer')
        with open(path, 'r') as f:
            return preprocess_csd_code(f.read()) + '\n'

    @property
    def ft_statements(self) -> str:
        s = StringIO()
        s.write('\n')
        path_wildcard = os.path.join(get_template_dir(), 'csd_ft_*.txt')
        for ft in glob(path_wildcard):
            with open(ft, 'r') as f:
                s.write(preprocess_csd_code(f.read()))
                s.write('\n')
        return s.getvalue()

    @property
    def zakinit(self) -> str:
        return f'zakinit {self.zak.aloc_i}, {self.zak.kloc_i}'

    @property
    def instr_va(self) -> str:
        s = StringIO()
        s.write('instr 1 ; Voice area\n')
        s.write('\n'.join([udo.get_statement() for udo in self.udos]))
        s.write('\nendin\n')
        return s.getvalue()

    @property
    def instr_fx(self):
        s = StringIO()
        s.write('instr 2 ; FX area\n')
        s.write('; TODO')
        s.write('\nendin\n')
        return s.getvalue()

    @property
    def udo_defs(self):
        udo_src = {u.get_name(): u.get_src() for u in self.udos}
        names = sorted(udo_src.keys())
        return '\n\n'.join([preprocess_csd_code(udo_src[n]) for n in names]) + '\n'
