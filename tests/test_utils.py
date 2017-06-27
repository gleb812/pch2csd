from unittest import TestCase, skip

from pch2csd.patch import ModuleParameters, Location, Module
from pch2csd.resources import ProjectData
from pch2csd.util import AttrEqMixin


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


@skip('Need new ReprStrMixin examples after adding more properties to the classes')
class TestReprStrMixin(TestCase):
    def setUp(self):
        self.data = ProjectData()

    def test_simple(self):
        m = Module(self.data, Location.VOICE_AREA, 1, 1)
        expected_str = 'Module(type=1, type_name=Keyboard, id=1, location=Location.VOICE_AREA)'
        self.assertEqual(str(m), expected_str)

    def test_repr_equal_to_str(self):
        m = Module(self.data, Location.VOICE_AREA, 1, 1)
        self.assertEqual(m.__repr__(), m.__str__())
