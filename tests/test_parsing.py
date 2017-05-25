import os
from unittest import TestCase

from pch2csd.parsing.parsing import parse_pch2
from pch2csd.parsing.structs import Module


def get_test_resource(path: str) -> str:
    return os.path.join(os.path.dirname(__file__), 'resources', path)

class TestParsing(TestCase):
    def test_gleb2_pch(self):
        gleb_2_pch = get_test_resource('Gleb2.pch2')
        expected_modules = [Module(4, 1, 'voice'), Module(96, 2, 'voice')]
        parsed = parse_pch2(gleb_2_pch)
        self.assertSequenceEqual(parsed.modules, expected_modules)
