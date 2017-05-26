from unittest import TestCase

from pch2csd.parsing.structs import ModuleParameters
from pch2csd.parsing.util import AttrEqMixin


class TestAttrEqMixin(TestCase):
    class ThreeSlots(AttrEqMixin):
        def __init__(self, one, two, three):
            self.one = one
            self.two = two
            self.three = three

    def test_different_args(self):
        o1 = self.ThreeSlots(1, [x for x in 'test'], [x * 2 for x in range(10)])
        o2 = self.ThreeSlots(1, [x for x in 'test'], [x * 2 for x in range(10)])
        self.assertTrue(o1.attrs_equal(o2))

    def test_overloaded_eq(self):
        o1 = ModuleParameters(1, 2, [(1, 32), (3, '43')])
        o2 = ModuleParameters(1, 2, [x for x in [(1, 32), (3, '43')]])
        self.assertTrue(o1 == o2)

    def test_modified_arg__fail(self):
        o1 = self.ThreeSlots(1, [x for x in 'test'], [x * 2 for x in range(10)])
        o2 = self.ThreeSlots(1, [x for x in 'test'], [x * 2 for x in range(10)])
        o2.one = 2
        self.assertFalse(o1.attrs_equal(o2))

    def test_extra_args__fail(self):
        o1 = self.ThreeSlots(1, [x for x in 'test'], [x * 2 for x in range(10)])
        o2 = self.ThreeSlots(1, [x for x in 'test'], [x * 2 for x in range(10)])
        o2.__dict__['mod'] = 1
        self.assertFalse(o1.attrs_equal(o2))

    def test_different_types__fail(self):
        o1 = self.ThreeSlots(1, [x for x in 'test'], [x * 2 for x in range(10)])
        o2 = ModuleParameters(1, 2, [x for x in [(1, 32), (3, '43')]])
        self.assertFalse(o1.attrs_equal(o2))
