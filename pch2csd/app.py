import argparse

import os
from tabulate import tabulate

import pch2csd
from pch2csd.csdgen import ZakSpace, Csd
from pch2csd.parse import parse_pch2
from pch2csd.patch import Patch, transform_in2in_cables
from pch2csd.resources import get_template_module_path, ProjectData


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


def validate_udo(param):
    print('UDO template validation is not implemented yet')


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
    p.cables = [transform_in2in_cables(p, c) for c in p.cables]
    zak = ZakSpace()
    try:
        udos = zak.connect_patch(p)
    except ValueError:
        exit(-1)
    csd = Csd(p, zak, udos)
    with open(csd_save_path, 'w') as f:
        f.write(csd.get_code())


def main():
    arg_parser = argparse.ArgumentParser(prog='pch2csd',
                                         description='convert Clavia Nord Modular G2 patches to the Csound code',
                                         epilog='Version {}, homepage: {}'.format(pch2csd.__version__,
                                                                                  pch2csd.__homepage__))
    arg_parser.add_argument('file', metavar='file', nargs=1, help='a patch or an UDO template file')
    arg_parser.add_argument('-p', '--parse', action='store_const', const=True,
                            help='parse the patch file and print its content')
    # arg_parser.add_argument('-u', '--validate-udo', action='store_const', const=True,
    #                               help="validate the UDO template file (overrides '-p')")
    arg_parser.add_argument('--version', action='version', version='%(prog)s ' + pch2csd.__version__)
    args = arg_parser.parse_args()
    # if args.validate_udo:
    #     validate_udo(args.file[0])
    if args.parse:
        print_pch2(args.file[0])
    else:
        convert_pch2(args.file[0])


if __name__ == '__main__':
    main()
