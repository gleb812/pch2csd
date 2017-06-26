from unittest import TestCase

from pch2csd.data import ProjectData
from pch2csd.parsing import parse_pch2
from tests.util import get_test_resource


class TestCableTracing(TestCase):
    def setUp(self):
        self.data = ProjectData()

    def test_tracing_3osc(self):
        pch2 = get_test_resource('test_3osc.pch2')
        patch = parse_pch2(self.data, pch2)
        print('lal')
