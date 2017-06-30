import argparse
import logging

import os
from tabulate import tabulate

import pch2csd
from pch2csd.csdgen import ZakSpace, Csd, UdoTemplate
from pch2csd.parse import parse_pch2
from pch2csd.patch import Patch, transform_in2in_cables
from pch2csd.resources import get_template_module_path, ProjectData
from tests.util import get_test_resource

log = logging.getLogger()


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


def validate_udo(type_id: int):
    print('Checking UDO for the type ID {}'.format(type_id))
    data = ProjectData()
    pch2_files = [get_test_resource(s) for s in ['test_all_modules_1.pch2',
                                                 'test_all_modules_2.pch2']]
    mod = None
    patch = None
    for p in map(lambda x: parse_pch2(data, x), pch2_files):
        for m in p.modules:
            if m.type == type_id:
                mod = m
                patch = p
                break
    if mod is None:
        log.error("Unknown module type '%d'", type_id)
        exit(-1)
    print('Found a module of this type: {}'.format(mod.type_name))
    if not os.path.isfile(get_template_module_path(type_id)):
        log.error('No UDO template file found for this module. Please, consider writing one.\n'
                  '      More info: https://github.com/gleb812/pch2csd/wiki/Making-a-new-module')
        exit(-1)
    udo = UdoTemplate(mod)
    if udo.udo_lines == [] and udo.args == [] and udo.maps == []:
        exit(-1)
    num_params = len(patch.find_mod_params(mod.location, mod.id).values)
    for args in udo.args:
        if len(args[0]) != num_params:
            log.error("The number of parameters in the 'args' UDO annotation "
                      "doesn't correspond to the actual number of parameters in this module type")
            exit(-1)
    for m in udo.maps:
        valid = True
        offset = 1 if m[0] == 'd' else 2
        for t in m[offset:]:
            if t not in data.value_maps:
                log.error("Unknown table '%s'", t)
                valid = False
        if not valid:
            exit(-1)
    print('The UDO template seems to be correct')


def print_pch2(fn: str):
    if not fn.lower().endswith('.pch2'):
        print("error: patch file should have extension '.pch2'")
        exit(-1)
    data = ProjectData()
    path = os.path.abspath(fn)
    patch = parse_pch2(data, path)

    mod_table = [['Name', 'ID', 'Type', 'Parameters', 'Area']]
    for m in patch.modules:
        p = patch.find_mod_params(m.location, m.id)
        mod_table.append([m.type_name,
                          m.id,
                          m.type,
                          str(p.values),
                          m.location.short_str()])
    cab_table = [['From', '', 'To', 'Color', 'Type', 'Area']]
    for c in patch.cables:
        mf_name = patch.find_module(c.module_from, c.loc).type_name
        mt_name = patch.find_module(c.module_to, c.loc).type_name
        pin1, pin2 = c.type.short_str().split('-')
        cab_table.append([
            '{}(id={}, {pin}={})'.format(mf_name, c.module_from, c.jack_from, pin=pin1),
            '->',
            '{}(id={}, {pin}={})'.format(mt_name, c.module_to, c.jack_to, pin=pin2),
            c.color.short_str(),
            c.type.short_str(),
            c.loc.short_str()])
    print('Patch file: {}\n'.format(os.path.basename(path)))
    print('Modules')
    print(tabulate(mod_table, headers='firstrow', tablefmt='simple'))
    print('\nCables')
    print(tabulate(cab_table, headers='firstrow', tablefmt='simple'))


def convert_pch2(fn: str):
    if not fn.lower().endswith('.pch2'):
        print("error: patch file should have extension '.pch2'")
        exit(-1)
    path = os.path.abspath(fn)
    dirname = os.path.dirname(path)
    csd_save_path = os.path.join(dirname, os.path.basename(path) + '.csd')
    data = ProjectData()
    p = parse_pch2(data, path)
    p.cables = [c for c in [transform_in2in_cables(p, c) for c in p.cables]
                if c is not None]
    zak = ZakSpace()
    try:
        udos = zak.connect_patch(p)
    except ValueError:
        exit(-1)
    csd = Csd(p, zak, udos)
    with open(csd_save_path, 'w') as f:
        f.write(csd.get_code())


def main():
    arg_parser = argparse.ArgumentParser(
        prog='pch2csd',
        description='convert Clavia Nord Modular G2 patches to the Csound code',
        epilog='Version {}, homepage: {}'.format(pch2csd.__version__, pch2csd.__homepage__))
    arg_parser.add_argument('arg', metavar='arg', nargs=1,
                            help='a pch2 file path or an UDO numerical ID')
    arg_parser.add_argument('-p', '--print', action='store_const', const=True,
                            help='parse the patch file and print its content')
    arg_parser.add_argument('-c', '--check-udo', action='store_const', const=True,
                            help="validate the UDO template file (overrides '-p')")
    arg_parser.add_argument('--version', action='version',
                            version='%(prog)s ' + pch2csd.__version__)
    args = arg_parser.parse_args()
    if args.check_udo:
        try:
            type_id = int(args.arg[0])
            validate_udo(type_id)
        except ValueError:
            log.error("You should pass the integer as the 'arg' parameter when using '--check-udo'")
    elif args.print:
        print_pch2(args.arg[0])
    else:
        convert_pch2(args.arg[0])


if __name__ == '__main__':
    main()
