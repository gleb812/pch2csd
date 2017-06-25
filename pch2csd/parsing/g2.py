from io import FileIO
from struct import unpack

from bitarray import bitarray

from pch2csd.parsing.structs import Patch, Module, Location, CableColor, CableType, Cable, \
    ModuleParameters
from pch2csd.parsing.util import BitArrayStream


def parse_header(pch2: FileIO, patch: Patch):
    while pch2.read(1) != b'\0':
        pass
    version, type = unpack('>BB', pch2.read(2))
    patch.ver = version
    patch.type = 'patch' if type == 0 else 'performance'


def parse_location(loc: int):
    return Location.from_int(loc >> 6)


def parse_module_list(blob: bitarray, patch: Patch):
    bits = BitArrayStream(blob)
    loc, num_modules = bits.read_ints([2, 8])
    num_modules, = unpack('>B', blob[2:10].tobytes())
    for i in range(num_modules):
        mod_type, mod_id = bits.read_ints([8, 8])
        hpos, vpos, color = bits.read_ints([7, 7, 8])
        _skip_, _insert_ = bits.read_ints([8, 4])
        for m in range(_insert_):
            _skip_ = bits.read_ints([6])
        mod = Module(Location.from_int(loc), mod_type, mod_id)
        patch.modules.append(mod)


def parse_cable_list(blob: bitarray, patch: Patch):
    location = parse_location(blob.tobytes()[0])
    num_cables, = unpack('>B', blob[16:24].tobytes())
    for i in range(24, 32 * num_cables, 32):
        color = CableColor.from_int(blob[i:i + 3].tobytes()[0] >> 5)
        module_from, = unpack('>B', blob[i + 3:i + 11].tobytes())
        jack_from = blob[i + 11:i + 17].tobytes()[0] >> 2
        cable_type = CableType.from_int(int(blob[i + 17]))
        module_to, = unpack('>B', blob[i + 18:i + 26].tobytes())
        jack_to = blob[i + 26:i + 32].tobytes()[0] >> 2
        patch.cables.append(
            Cable(location, cable_type, color, module_from, jack_from, module_to, jack_to))


def parse_module_parameters(blob: bitarray, patch: Patch):
    bits = BitArrayStream(blob)
    loc, num_modules, num_variations = bits.read_ints([2, 8, 8])
    for imod in range(num_modules):
        mod_id, num_params = bits.read_ints([8, 7])
        mod_params = ModuleParameters(Location.from_int(loc), mod_id, num_params)
        for ivar in range(num_variations):
            var, *p_list = bits.read_ints([8] + [7] * num_params)
            if var == patch.description.active_variation:
                mod_params.values.extend(p_list)  # TODO variations support
        patch.mod_params.append(mod_params)


def parse_data_object(head: int, blob: bitarray, patch: Patch, ctx: dict):
    if head == 0x4a:
        parse_module_list(blob, patch)
    elif head == 0x52:
        parse_cable_list(blob, patch)
    elif head == 0x4d:
        if ctx['head_4d_count'] > 0:
            # skipping patch settings
            parse_module_parameters(blob, patch)
        ctx['head_4d_count'] += 1


def parse_pch2(pch2_file: str) -> Patch:
    patch = Patch()
    with open(pch2_file, 'rb') as pch2:
        parse_header(pch2, patch)
        context = {'head_4d_count': 0}
        while True:
            object_header = pch2.read(3)
            if len(object_header) == 3:
                obj_header, obj_len = unpack('>BH', object_header)
                blob = pch2.read(obj_len)
                blob_bits = bitarray(endian='big')
                blob_bits.frombytes(blob)
                parse_data_object(obj_header, blob_bits, patch, context)
            else:
                break
    return patch
