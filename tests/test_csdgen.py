from unittest import TestCase

from pch2csd.csdgen.structs import UdoStatement
from pch2csd.data import ProjectData
from pch2csd.parsing import parse_pch2
from tests.util import get_test_resource, cmp_str_lines


class TestPolymorphism(TestCase):
    def setUp(self):
        self.data = ProjectData()
        self.poly_mix2_patch = parse_pch2(self.data, get_test_resource('test_poly_mix2.pch2'))
        self.udo_mix2_k = """;@ iiiii, kkk, k
opcode Mix21A, 0, iiiiiiiii   ; MULTIMODE support a/k?
; TODO: lin/log scale, chain input
iLev1, iSw1, iLev2, iSw2, iScale, izIn1, izIn2, izInChain, izOut xin
k1 zkr izIn1
k2 zkr izIn2
k3 zkr izInChain
aout = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2
zkw aout, izOut
endop"""
        self.udo_mix2_a = """;@ iii, aaa, a
opcode Mix21A, 0, iiiiiiiii   ; MULTIMODE support a/k?
; TODO: lin/log scale, chain input
iLev1, iSw1, iLev2, iSw2, iScale, izIn1, izIn2, izInChain, izOut xin
k1 zar izIn1
k2 zar izIn2
k3 zar izInChain
aout = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2
zaw aout, izOut
endop"""

    def test_mix2__choose_right_templates(self):
        p = self.poly_mix2_patch
        udo_s = [UdoStatement(p, m) for m in p.modules][:2]
        self.assertSequenceEqual([s.get_udo_name() for s in udo_s],
                                 ['Mix21A_v0', 'Mix21A_v1'])
        self.assertTrue(cmp_str_lines(udo_s[0].get_udo_code(), self.udo_mix2_k))
        self.assertTrue(cmp_str_lines(udo_s[1].get_udo_code(), self.udo_mix2_a))
