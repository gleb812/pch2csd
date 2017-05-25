from io import FileIO
from struct import unpack

from bitarray import bitarray

from pch2csd.parsing.structs import Patch, Module


def parse_header(pch2: FileIO, patch: Patch):
    while pch2.read(1) != b'\0':
        pass
    version, type = unpack('>BB', pch2.read(2))
    patch.ver = version
    patch.type = 'patch' if type == 0 else 'performance'


def parse_patch_description(pch2):
    raise NotImplementedError()


def parse_module_list(blob: bytes, patch: Patch):
    blob_bits = bitarray(endian='big')
    blob_bits.frombytes(blob)
    location = 'voice' if blob_bits.tobytes()[0] >> 6 == 1 else 'fx'
    num_modules, = unpack('>B', blob_bits[2:10].tobytes())
    for i in range(10, 50 * num_modules, 50):
        mod_type, mod_id = unpack('>BB', blob_bits[i:i + 16].tobytes())
        patch.modules.append(
            Module(mod_type, mod_id, location))


def parse_data_object(head: int, blob: bytes, patch: Patch):
    if head == 0x4a:
        parse_module_list(blob, patch)


def parse_pch2(pch2_file: str) -> Patch:
    patch = Patch()
    with open(pch2_file, 'rb') as pch2:
        parse_header(pch2, patch)
        while True:
            object_header = pch2.read(3)
            if len(object_header) == 3:
                obj_header, obj_len = unpack('>BH', object_header)
                parse_data_object(obj_header, pch2.read(obj_len), patch)
            else:
                break
    return patch
