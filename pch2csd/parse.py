from io import FileIO
from struct import unpack

from bitarray import bitarray

from pch2csd.patch import Patch, Module, Location, CableColor, CableType, Cable, ModuleParameters, \
    transform_in2in_cables
from pch2csd.resources import ProjectData
from pch2csd.util import BitArrayStream


def parse_header(pch2: FileIO, patch: Patch):
    while pch2.read(1) != b'\0':
        pass
    version, h_type = unpack('>BB', pch2.read(2))
    patch.ver = version
    patch.type = 'patch' if h_type == 0 else 'performance'


def parse_location(loc: int):
    return Location.from_int(loc >> 6)


def parse_module_list(blob: bitarray, patch: Patch):
    bits = BitArrayStream(blob)
    loc, num_modules = bits.read_ints([2, 8])
    num_modules, = unpack('>B', blob[2:10].tobytes())
    for i in range(num_modules):
        mod_type, mod_id = bits.read_ints([8, 8])
        hpos, vpos, color = bits.read_ints([7, 7, 8])
        _ = bits.read_ints([8])
        num_modes = bits.read_ints([4])[0]
        modes = bits.read_ints([6] * num_modes)
        mod = Module(patch.data, Location.from_int(loc), mod_type, mod_id, modes)
        patch.modules.append(mod)


def parse_cable_list(blob: bitarray, patch: Patch):
    bits = BitArrayStream(blob)
    loc, _skip_, num_cables = bits.read_ints([2, 14, 8])
    for i in range(num_cables):
        color, module_from, jack_from, cable_type, module_to, jack_to = bits.read_ints(
            [3, 8, 6, 1, 8, 6])
        c = Cable(Location.from_int(loc), CableType.from_int(cable_type),
                  CableColor.from_int(color),
                  module_from, jack_from, module_to, jack_to)
        if c.type == CableType.IN_TO_IN:
            # By some reason, in2in cables have sources swapped back with destinations...
            c.module_from = module_to
            c.module_to = module_from
            c.jack_from = jack_to
            c.jack_to = jack_from
        patch.cables.append(c)


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


def parse_pch2(data: ProjectData, pch2_file: str, convert_in2in=True) -> Patch:
    patch = Patch(data)
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
    if convert_in2in:
        patch.cables = [c for c in [transform_in2in_cables(patch, c)
                                    for c in patch.cables]
                        if c is not None]
    return patch
