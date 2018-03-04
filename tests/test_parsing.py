from unittest import TestCase

from pch2csd.parse import parse_pch2
from pch2csd.patch import Module, Location, Cable, CableType, CableColor, \
    ModuleParameters, transform_in2in_cables
from pch2csd.resources import ProjectData
from tests.util import get_test_resource


class TestParsing(TestCase):
    def setUp(self):
        self.data = ProjectData()

    def test_gleb2_pch(self):
        gleb_2_pch = get_test_resource('Gleb2.pch2')
        expected_modules = [Module(self.data, Location.VOICE_AREA, 4, 1),
                            Module(self.data, Location.VOICE_AREA, 96, 2, [2])]
        expected_cables = [Cable(Location.VOICE_AREA, CableType.IN_TO_IN,
                                 CableColor.WHITE, 1, 1, 1, 0)]
        expected_mod_params = [ModuleParameters(Location.VOICE_AREA, 1, 3, [0, 1, 0]),
                               ModuleParameters(Location.VOICE_AREA, 2, 5, [127, 127, 0, 2, 0])]
        parsed = parse_pch2(self.data, gleb_2_pch, convert_in2in=False)
        self.assertSequenceEqual(parsed.modules, expected_modules)
        self.assertSequenceEqual(parsed.cables, expected_cables)
        self.assertSequenceEqual(parsed.mod_params, expected_mod_params)

    def test_gleb2_pch__unequal(self):
        gleb_2_pch = get_test_resource('Gleb2.pch2')
        expected_modules_correct = [Module(self.data, Location.VOICE_AREA, 4, 1),
                                    Module(self.data, Location.VOICE_AREA, 96, 2, [2])]
        expected_modules = [Module(self.data, Location.VOICE_AREA, 4, 1),
                            Module(self.data, Location.VOICE_AREA, 100, 2)]
        expected_cables = [
            Cable(Location.VOICE_AREA, CableType.IN_TO_IN, CableColor.WHITE, 1, 0, 141, 1)]
        parsed = parse_pch2(self.data, gleb_2_pch, convert_in2in=False)
        self.assertTrue(parsed.modules == expected_modules_correct)
        self.assertFalse(parsed.modules == expected_modules)
        self.assertFalse(parsed.cables == expected_cables)


class TestCableTracing(TestCase):
    def setUp(self):
        self.data = ProjectData()

    def test_in2in__all_cables_from_the_first_module(self):
        pch2 = get_test_resource('test_in2in.pch2')
        patch = parse_pch2(self.data, pch2)
        patch.cables = [transform_in2in_cables(patch, c) for c in patch.cables]
        for c in patch.cables:
            self.assertEqual(c.module_from, 1)


class TestModes(TestCase):
    def setUp(self):
        self.data = ProjectData()

    def test_modes__LfoC(self):
        pch2 = get_test_resource('test_modes_LfoC.pch2')
        patch = parse_pch2(self.data, pch2)
        p_modes = [m.modes for m in patch.modules]
        self.assertSequenceEqual(p_modes, [[0], [1], [2], [3], [4], [5], [6], [7]])
