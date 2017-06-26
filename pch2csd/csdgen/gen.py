import os

from io import StringIO
from typing import Optional

from pch2csd.data import get_template_module_path
from pch2csd.parsing.structs import Patch


def _all_modules_implemented(patch: Patch):
    not_implemented = [x.type_name for x in patch.modules
                       if not os.path.isfile(get_template_module_path(x.type))]
    if len(not_implemented) > 0:
        print('The patch file contains some modules that has not been implemented yet:')
        print(', '.join(not_implemented))
        print('Please, consider contributing these modules, following our tutorial:')
        print('https://github.com/gleb812/pch2csd/wiki/How-to-add-new-modules')
        return False
    return True


def generate_csd(patch: Patch) -> Optional[str]:
    string_buf = StringIO()
    if not _all_modules_implemented(patch):
        return None

    return string_buf.getvalue()
