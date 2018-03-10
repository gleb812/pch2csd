import os
from copy import deepcopy
from glob import glob
from io import StringIO
from typing import List, Dict, Tuple

from tabulate import tabulate

from .patch import Patch, Cable, ModuleK2A, ModuleA2K, CableType, \
    Location, Module, CableColor
from .resources import get_template_path, get_template_dir
from .udo import UdoTemplate
from .util import preprocess_csd_code


class Udo:
    def __init__(self, patch: Patch, mod: Module):
        self.patch = patch
        self.mod = mod
        self.tpl = UdoTemplate(mod)
        if not self.tpl.validate(patch.data):
            raise ValueError(
                "error ({0}.txt): the UDO '{1}' is invalid\n"
                "please, run 'pch2csd -c {0}' for details".format(
                    self.mod.type, self.mod.type_name))

        ins = self.tpl.ins
        outs = self.tpl.outs

        self.udo_variant = self._choose_udo_variant(len(ins))
        self.in_types = ins[self.udo_variant].types if ins != [] else []
        self.out_types = outs[self.udo_variant].types if outs != [] else []

        self.inlets, self.outlets = self._init_zak_connections()

    def __repr__(self):
        return '{}(type={}, id={})'.format(self.get_name(), self.mod.type, self.mod.id)

    def get_name(self) -> str:
        if len(self.tpl.opcodes) < 2:
            return self.mod.type_name
        else:
            return '{}_v{}'.format(self.mod.type_name, self.udo_variant)

    def get_src(self) -> str:
        src = list(self.tpl.opcodes[self.udo_variant].src)
        assert src[0].startswith('opcode')
        src[0] = src[0].replace(self.mod.type_name,
                                self.get_name())
        return '\n'.join(src)

    def _choose_udo_variant(self, len_ins) -> int:
        if len_ins == 0:
            return 0
        v = 0
        cables = self.patch.find_all_incoming_cables(self.mod.location, self.mod.id)
        if len(self.tpl.opcodes) < 2 or cables is None:
            return v
        cs_rates = {c.jack_to: CableColor.to_cs_rate_char(c.color) for i, c in enumerate(cables)}
        tpl_v0_ins = self.tpl.ins[0].types
        tpl_v1_ins = self.tpl.ins[1].types
        for i, r in cs_rates.items():
            if tpl_v0_ins[i] != r and tpl_v1_ins[i] == r:
                v = 1
                break
        return v

    def get_params(self) -> List[float]:
        params = self.patch.find_mod_params(self.mod.location, self.mod.id)
        if params is not None and len(self.tpl.maps) != len(params.values):
            print("warning: template '{}' has different number of map annotation "
                  "than it is parameters found in the parsed module '{}'. "
                  "Returning -1s for now.".format(self.tpl, self.mod))
            return [-1] * params.num_params
        if params is None:
            return []
        else:
            return [self._map_value(i, v, params.values) for i, v in enumerate(params.values)]

    def get_statement_parts(self) -> Tuple[str, str, str, str, str]:
        name, params, modes, inlets, outlets = self.get_name(), '', '', '', ''
        params = ','.join([str(f) for f in self.get_params()])
        modes = ','.join([str(m) for m in self.mod.modes])
        inlets = ','.join([str(i) for i in self.inlets])
        outlets = ','.join(str(i) for i in self.outlets)

        # TODO refactor this mess
        if params != '' and (inlets != '' or outlets != '' or modes != ''):
            params += ','
        if modes != '' and (inlets != '' or outlets != ''):
            modes += ','
        if inlets != '' and outlets != '':
            inlets += ','

        return name, params, modes, inlets, outlets

    def _init_zak_connections(self):
        ins, outs = self.in_types, self.out_types
        return [ZakSpace.trash_bus] * len(ins), [ZakSpace.zero_bus] * len(outs)

    def _map_value(self, i, v, all_vals):
        m = self.tpl.maps[i]
        if m.map_type == 'd':
            table = self.patch.data.value_maps[m.tables[0]]
        elif m.map_type == 's':
            ref = m.switch_ref
            try:
                if ref.isdigit():
                    table_name = m.tables[all_vals[int(ref) - 1]]
                else:
                    ms = [a for a in self.tpl.maps if a.name == ref]
                    if len(ms) == 1:
                        table_name = m.tables[all_vals[ms[0].idx]]
                    else:
                        raise Exception
            except Exception:
                raise ValueError("error ({}, line {}): couldn't retrieve map".format(
                    self.tpl.filename,
                    self.tpl.maps[i].line + 1))
            table = self.patch.data.value_maps[table_name]
        else:
            raise ValueError('Mapping type {} is not supported'.format(m[0]))
        return table[v]


class ZakSpace:
    zero_bus = 0
    trash_bus = 1

    def __init__(self):
        self.aloc_i = 6
        self.kloc_i = 6
        self.zakk = {}  # Tuple[mod_id, inlet_id] -> zak_loc
        self.zaka = {}  # Tuple[mod_id, inlet_id] -> zak_loc

    def _init_zak(self):
        self.aloc_i = 6
        self.kloc_i = 6
        self.zakk = {}  # Tuple[mod_id, inlet_id] -> zak_loc
        self.zaka = {}  # Tuple[mod_id, inlet_id] -> zak_loc

    def connect_patch(self, p: Patch) -> List[Udo]:
        self._init_zak()
        udos = deepcopy({m.id: Udo(p, m) for m in p.modules})
        for c in p.cables:
            mf, jf, mt, jt = c.module_from, c.jack_from, c.module_to, c.jack_to
            try:
                if udos[mf].out_types[jf] == udos[mt].in_types[jt]:
                    self._zak_connect_direct(c, udos)
                else:
                    self._zak_connect_convert_rates(c, udos, p)
            except IndexError:
                raise ValueError("error: couldn't connect "
                                 "module with id {} (outlet {}) to "
                                 "module with id {} (outlet {})"
                                 .format(mf, jf, mt, jt))
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
            raise ValueError('Unknown rate type: {}'.format(rate_type))
        out_id = (mf, jf)
        zak = self.zakk if rate_type == 'k' else self.zaka
        if out_id not in zak:
            zak[out_id] = self._kloc_new() if rate_type == 'k' else self._aloc_new()
        zak_id = zak[out_id]
        udos[mf].outlets[jf] = zak_id
        udos[mt].inlets[jt] = zak_id

    def _zak_connect_convert_rates(self, c: Cable, udos: Dict[int, Udo], p: Patch):
        mf, jf, mt, jt = c.module_from, c.jack_from, c.module_to, c.jack_to
        rate_from, rate_to = udos[mf].out_types[jf], udos[mt].in_types[jt]
        converter = ModuleK2A(p.data, c.loc) if rate_from == 'k' else ModuleA2K(p.data, c.loc)
        u = udos.setdefault(converter.id, Udo(p, converter))
        c1 = Cable(c.loc, CableType.OUT_TO_IN, converter.get_io_cable_colors()[0],
                   mf, jf, converter.id, 0)
        c2 = Cable(c.loc, CableType.OUT_TO_IN, converter.get_io_cable_colors()[1],
                   converter.id, 0, mt, jt)
        self._zak_connect_direct(c1, udos)
        self._zak_connect_direct(c2, udos)


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
        s = StringIO()
        s.write('gkNote init 64\n')
        s.write('gkGate init 0\n')
        s.write('zakinit {}, {}\n'.format(self.zak.aloc_i, self.zak.kloc_i))
        s.write('massign 1,0\n')
        s.write('massign 2,0\n')
        s.write('massign 3,0\n')
        s.write('massign 4,0\n')
        return s.getvalue()

    def _gen_instr(self, loc: Location) -> str:
        s = StringIO()
        s.write('; --------------------\n')
        s.write('; {} AREA\n'.format(loc.short_str()))
        s.write('instr {}\n'.format(2 - loc.value))
        statements = [udo.get_statement_parts() for udo in self.udos
                      if udo.mod.location == loc]
        table_head = ('; Module', 'Parameters', 'Modes', 'Inlets', 'Outlets')
        table_str = tabulate(statements, table_head, tablefmt='plain')
        s.write(table_str)
        s.write('\nendin\n')
        return s.getvalue()

    @property
    def instr_va(self):
        return self._gen_instr(Location.VOICE_AREA)

    @property
    def instr_fx(self):
        return self._gen_instr(Location.FX_AREA)

    @property
    def udo_defs(self):
        udo_src = {u.get_name(): u.get_src() for u in self.udos}
        names = sorted(udo_src.keys())
        return '\n\n'.join([preprocess_csd_code(udo_src[n]) for n in names]) + '\n'
