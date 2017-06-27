from pch2csd.data import ProjectData
from pch2csd.parsing.structs import Patch


class ZakSpace:
    def __init__(self, data: ProjectData, patch: Patch):
        self.data = data
        self.patch = patch
