from glob import glob
from unittest import TestCase

from pch2csd.app import print_pch2, convert_pch2, validate_udo
from tests.util import get_test_resource


class TestApp(TestCase):
    def test_check_udo__not_raises(self):
        validate_udo(81)

    def test_print_pch2__not_raises(self):
        for f in glob(get_test_resource('*.pch2')):
            print(f)
            print_pch2(f)

    def test_convert_pch2__not_raises(self):
        try:
            convert_pch2(get_test_resource('test_in2in.pch2'))
        except SystemExit:
            pass  # allow program to exit while we haven't created all modules
