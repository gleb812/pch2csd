from unittest import TestCase, skip

from pch2csd.csdgen.structs import Udo, Csd, ZakSpace
from pch2csd.data import ProjectData
from pch2csd.parsing import parse_pch2
from pch2csd.parsing.g2 import transform_in2in_cables
from tests.util import get_test_resource, cmp_str_lines


class TestPolymorphism(TestCase):
    def setUp(self):
        self.data = ProjectData()
        self.poly_mix2 = parse_pch2(self.data, get_test_resource('test_poly_mix2.pch2'))
        self.udo_mix2_k = """opcode Mix21A_v0, 0, iiiiiiiii   ; MULTIMODE support a/k?
; TODO: lin/log scale, chain input
iLev1, iSw1, iLev2, iSw2, iScale, izIn1, izIn2, izInChain, izOut xin
k1 zkr izIn1
k2 zkr izIn2
k3 zkr izInChain
aout = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2
zkw aout, izOut
endop
"""
        self.udo_mix2_a = """opcode Mix21A_v1, 0, iiiiiiiii   ; MULTIMODE support a/k?
; TODO: lin/log scale, chain input
iLev1, iSw1, iLev2, iSw2, iScale, izIn1, izIn2, izInChain, izOut xin
k1 zar izIn1
k2 zar izIn2
k3 zar izInChain
aout = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2
zaw aout, izOut
endop
"""

    def test_mix2__choose_right_templates(self):
        p = self.poly_mix2
        udo_s = [Udo(p, m) for m in p.modules][:2]
        self.assertSequenceEqual([s.get_name() for s in udo_s],
                                 ['Mix21A_v0', 'Mix21A_v1'])
        self.assertTrue(cmp_str_lines(udo_s[0].get_src(), self.udo_mix2_k))
        self.assertTrue(cmp_str_lines(udo_s[1].get_src(), self.udo_mix2_a))


@skip('Parameter mapping is not implemented yet')
class TestParameterMapping(TestCase):
    def setUp(self):
        self.data = ProjectData()
        self.poly_mix2 = parse_pch2(self.data, get_test_resource('test_poly_mix2.pch2'))

    def test_poly_mix2(self):
        p = self.poly_mix2
        udo_s = [Udo(p, m) for m in p.modules]
        params = [udo.get_params() for udo in udo_s]


class TestUdoGen(TestCase):
    def setUp(self):
        self.data = ProjectData()
        self.poly_mix2_fn = get_test_resource('test_poly_mix2.pch2')

    def test_udo_statement_gen(self):
        p = parse_pch2(self.data, self.poly_mix2_fn)
        p.cables = [transform_in2in_cables(p, c) for c in p.cables]
        zak = ZakSpace()
        udos = zak.connect_patch(p)
        csd = Csd(p, zak, udos)
        print(csd.get_code())
