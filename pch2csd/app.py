import argparse
import os

from tabulate import tabulate

from pch2csd import __version__, __homepage__
from pch2csd.csdgen import ZakSpace, Csd, UdoTemplate
from pch2csd.parse import parse_pch2
from pch2csd.patch import Patch, transform_in2in_cables
from pch2csd.resources import get_template_module_path, ProjectData


def get_test_resource(path: str) -> str:
    return os.path.abspath(
        os.path.join(os.path.dirname(__file__), '..', 'tests', 'resources', path))


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
    print("validating template for the module of type '{id}' ({id}.txt)".format(id=type_id))
    pch2_files = [get_test_resource(s) for s in ['test_all_modules_1.pch2',
                                                 'test_all_modules_2.pch2']]
    data, mod, patch = ProjectData(), None, None
    for p in map(lambda x: parse_pch2(data, x), pch2_files):
        for m in p.modules:
            if m.type == type_id:
                mod, patch = m, p
                break
    if mod is not None:
        print('found a module of this type: {}'.format(mod.type_name))
        udo = UdoTemplate(mod)
        valid = udo.validate(data)
    else:
        print("error: unknown module type '%d'", type_id)


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
            '{}(id={}, {}={})'.format(mf_name, c.module_from, pin1, c.jack_from),
            '->',
            '{}(id={}, {}={})'.format(mt_name, c.module_to, pin2, c.jack_to),
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
        print("error: the patch file should have extension '.pch2'")
        exit(-1)
    data = ProjectData()
    path = os.path.abspath(fn)
    p = parse_pch2(data, path)
    p.cables = [c for c in [transform_in2in_cables(p, c) for c in p.cables]
                if c is not None]
    zak = ZakSpace()
    try:
        udos = zak.connect_patch(p)
    except ValueError:
        exit(-1)
    csd = Csd(p, zak, udos)
    dirname = os.path.dirname(path)
    csd_save_path = os.path.join(dirname, os.path.basename(path) + '.csd')
    with open(csd_save_path, 'w') as f:
        f.write(csd.get_code())


def main():
    arg_parser = argparse.ArgumentParser(
        prog='pch2csd',
        description='convert Clavia Nord Modular G2 patches to the Csound code',
        epilog='Version {}, homepage: {}'.format(__version__, __homepage__))
    arg_parser.add_argument('arg', metavar='arg', nargs=1,
                            help='a pch2 file path or an UDO numerical ID')
    arg_parser.add_argument('-p', '--print', action='store_const', const=True,
                            help='parse the patch file and print its content')
    arg_parser.add_argument('-c', '--check-udo', action='store_const', const=True,
                            help="validate the UDO template file (overrides '-p')")
    arg_parser.add_argument('--version', action='version',
                            version='%(prog)s ' + __version__)
    args = arg_parser.parse_args()
    if args.check_udo:
        try:
            type_id = int(args.arg[0])
            validate_udo(type_id)
        except ValueError:
            print("you should pass the integer as the 'arg' parameter when using '--check-udo'")
    elif args.print:
        print_pch2(args.arg[0])
    else:
        convert_pch2(args.arg[0])


if __name__ == '__main__':
    main()
