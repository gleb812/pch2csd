from pch2csd.parse import parse_pch2
from pch2csd.resources import ProjectData
from pch2csd.util import get_test_resource

all_module_pch2 = None


def get_all_module_pch2():  # -> List[Patch]
    global all_module_pch2
    if all_module_pch2 is None:
        data = ProjectData()
        p = [parse_pch2(data, f)
             for f in [get_test_resource('test_all_modules_1.pch2'),
                       get_test_resource('test_all_modules_2.pch2')]]
        all_module_pch2 = p
    return all_module_pch2
