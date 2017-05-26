import os
from unittest import TestCase

from pch2csd.parsing.g2 import parse_pch2
from pch2csd.parsing.structs import Module, Location, Cable, CableType, CableColor


def get_test_resource(path: str) -> str:
    return os.path.join(os.path.dirname(__file__), 'resources', path)


class TestParsing(TestCase):
    def test_gleb2_pch(self):
        gleb_2_pch = get_test_resource('Gleb2.pch2')
        expected_modules = [Module(Location.VOICE_AREA, 4, 1),
                            Module(Location.VOICE_AREA, 96, 2)]
        expected_cables = [
            Cable(Location.VOICE_AREA, CableType.IN_TO_IN, CableColor.WHITE, 1, 0, 1, 1)]
        parsed = parse_pch2(gleb_2_pch)
        self.assertSequenceEqual(parsed.modules, expected_modules)
        self.assertSequenceEqual(parsed.cables, expected_cables)

    def test_gleb2_pch__unequal(self):
        gleb_2_pch = get_test_resource('Gleb2.pch2')
        expected_modules_correct = [Module(Location.VOICE_AREA, 4, 1),
                                    Module(Location.VOICE_AREA, 96, 2)]
        expected_modules = [Module(Location.VOICE_AREA, 4, 1),
                            Module(Location.VOICE_AREA, 123, 2)]
        expected_cables = [
            Cable(Location.VOICE_AREA, CableType.IN_TO_IN, CableColor.WHITE, 1, 0, 141, 1)]
        parsed = parse_pch2(gleb_2_pch)
        self.assertTrue(parsed.modules == expected_modules_correct)
        self.assertFalse(parsed.modules == expected_modules)
        self.assertFalse(parsed.cables == expected_cables)
