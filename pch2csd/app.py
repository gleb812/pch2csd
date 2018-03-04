import argparse
import os
import sys
from io import StringIO

from tabulate import tabulate

from pch2csd import __version__, __homepage__
from pch2csd.csdgen import ZakSpace, Csd, UdoTemplate, UdoTemplateValidation, Udo
from pch2csd.parse import parse_pch2
from pch2csd.patch import Patch, Location
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


def validate_udo(type_id: int, io=sys.stdout, print_action=True):
    if print_action:
        print("checking module type '{id}' ({id}.txt)".format(id=type_id),
              file=io)
    pch2_files = [get_test_resource(s) for s in ['test_all_modules_1.pch2',
                                                 'test_all_modules_2.pch2']]
    data, mod, patch = ProjectData(), None, None
    for p in map(lambda x: parse_pch2(data, x), pch2_files):
        for m in p.modules:
            if m.type == type_id:
                mod, patch = m, p
                break
    if mod is not None:
        if print_action:
            print('module name: {}'.format(mod.type_name), file=io)
        udo = UdoTemplate(mod)
        v = UdoTemplateValidation(data, udo)
        v.print_errors(io)
        return v.is_valid(with_todos=True)
    else:
        print("error: unknown module type '{}'".format(type_id), file=io)
        return False


def print_module(fn_pch2: str, mod_id: int, loc: Location):
    if not fn_pch2.lower().endswith('.pch2'):
        print("error: patch file should have extension '.pch2'")
        exit(-1)
    data = ProjectData()
    path = os.path.abspath(os.path.expanduser(fn_pch2))
    p = parse_pch2(data, path)

    m = p.find_module(mod_id, loc)
    if m is None:
        print('error: cannot find module with id {} in the {} location'.format(mod_id, loc.short_str()))
        exit(-1)

    udo = Udo(p, m)
    params_midi = p.find_mod_params(loc, mod_id)
    params_mapped = udo.get_params()
    assert params_midi.num_params == len(params_mapped)

    tbl = [['Type', 'Raw', 'Mapped']]
    for raw, mapped in zip(params_midi.values, params_mapped):
        tbl.append(['Parameter', str(raw), str(mapped)])
    for mode in m.modes:
        tbl.append(['Mode', str(mode), ''])

    print('Patch: {}'.format(fn_pch2))
    print('Details of the module:\n{}'.format(m))
    print()
    print(tabulate(tbl, headers='firstrow', tablefmt='simple'))


def print_pch2(fn: str):
    if not fn.lower().endswith('.pch2'):
        print("error: patch file should have extension '.pch2'")
        exit(-1)
    data = ProjectData()
    path = os.path.abspath(os.path.expanduser(fn))
    patch = parse_pch2(data, path)

    mod_table = [['Name', 'ID', 'Type', 'Parameters', 'Modes', 'Area']]
    for m in patch.modules:
        p = patch.find_mod_params(m.location, m.id)
        mod_table.append([m.type_name,
                          m.id,
                          m.type,
                          str(p.values),
                          str(m.modes),
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
    path = os.path.abspath(os.path.expanduser(fn))
    p = parse_pch2(data, path)
    zak = ZakSpace()
    try:
        udos = zak.connect_patch(p)
    except ValueError as e:
        print('error: {}'.format(e))
        exit(-1)
    csd = Csd(p, zak, udos)
    dirname = os.path.dirname(path)
    csd_save_path = os.path.join(dirname, os.path.basename(path) + '.csd')
    with open(csd_save_path, 'w') as f:
        f.write(csd.get_code())


def gen_udo_status_doc():
    tpl_url = 'https://github.com/gleb812/pch2csd/blob/master/pch2csd/resources/templates/modules/{}.txt'
    data = ProjectData()
    with open('Module-implementation-status.md', 'w') as md:
        md.write('This file is automatically generated.\n\n')
        md.write('| Template | Module name | Status |\n')
        md.write('|----------|-------------|--------|\n')
        for p in [parse_pch2(data, get_test_resource(pch2file)) for pch2file
                  in ['test_all_modules_1.pch2', 'test_all_modules_2.pch2']]:
            for m in p.modules:
                status = StringIO()
                validate_udo(m.type, status, print_action=False)
                md.write('| [{}]({}) | {} | {} |\n'.format(
                    '{}.txt'.format(m.type),
                    tpl_url.format(m.type),
                    m.type_name,
                    '<br>'.join(status.getvalue().splitlines())))


def main():
    arg_parser = argparse.ArgumentParser(
        prog='pch2csd',
        description='convert Clavia Nord Modular G2 patches to the Csound code',
        epilog='Version {}, homepage: {}'.format(__version__, __homepage__))
    arg_parser.add_argument('arg', metavar='arg', nargs='?', default='patch.pch2',
                            help='a pch2 file path or an UDO numerical ID')
    arg_parser.add_argument('-d', '--debug', action='store_const', const=True,
                            help='print a stack trace in case of error')
    group = arg_parser.add_mutually_exclusive_group()
    group.add_argument('-p', '--print', action='store_const', const=True,
                       help='parse the patch file and print its content')
    group.add_argument('-m', '--mod-print', nargs=2, metavar=('module_id', '{voice,fx}'),
                       help='print extensive information about the module in'
                            'the file {arg}. You should provide two values:'
                            'an integer module id and an area.')
    group.add_argument('-c', '--check-udo', action='store_const', const=True,
                       help="validate the UDO template file (overrides '-p')")
    group.add_argument('-v', '--version', action='version',
                       version='%(prog)s ' + __version__)
    group.add_argument('-e', action='store_const', const=True,
                       help='show the elephant and exit')
    args = arg_parser.parse_args()
    if args.check_udo:
        try:
            type_id = int(args.arg)
            validate_udo(type_id)
        except ValueError:
            print("you should pass the integer as the 'arg' parameter when using '--check-udo'")
    elif args.mod_print:
        print_module(args.arg, int(args.mod_print[0]), Location.from_str(args.mod_print[1]))
    elif args.print:
        print_pch2(args.arg)
    elif args.e:
        show_elephant()
    else:
        if args.arg == 'gen_udo_status_doc':
            gen_udo_status_doc()
        else:
            try:
                convert_pch2(args.arg)
            except Exception as e:
                print(e)
                if args.debug:
                    import traceback
                    _, _, tb = sys.exc_info()
                    print()
                    print('-----------')
                    traceback.print_tb(tb, file=sys.stdout)


def show_elephant():
    print('///////////////////osyyo///////////////////////////////////////////////////////')
    print('//////////////+oshmNMMMmNmhyso+//////////////////+++++////////////////////+o///')
    print('///////////+oshydNMMMMMMMMMMMNh++++++++++ossssyysssyshhys+//////////////+hNmhys')
    print('/////////+oydmmNNNNNNNNNNNMMNNdhyyyyyyyhhddy+++::/ooossyyhs+///////////omMMMNNN')
    print('///////+oyyhhhdhhhhhhdmdmmddhyshhyysys++ossys+--+syyyyyysoo++/////////+hmmmmmdy')
    print('///+++++++++ooooooosossssoo+++syyyssyyss+-..`.ydmmddyo+/+++/++++++++++shhhhhyys')
    print('+++                        oooyhyyhyyyhhdso+/:sddyo+//++/////++++++++++ooosssss')
    print('+++ Clavia Nord Modular G2 sshhhyyyyyys+-+hho/ys+///++/////:+++++++++++++++++++')
    print('+++     Patch Converter    ooossosyyy+:``.--`.//+/+/://+/o+++++++++++++++++++++')
    print('+++                        oo+oysysso/:-.``````.-:/+/-/+syso+++++++++++++++++++')
    print('++oooooooooooooooooooooooooooosssysoosys+-``` ``-:////://oosooooooooo++++++++++')
    print('ooooooooooooosssssooosssssssssshyyso+shdh.`    `-/:-:-:--/++ooooooooooooooooyso')
    print('ssssssssyyyyyyyyyyyyyyyyyssssooooso+++yhh-     .:/--````-::-/oooooooooooosyhhdd')
    print('ossosssssssssssssssssssssssss/++++/--/+hs`   `.`-...````-..`oooooooosssyssssyyy')
    print('ooooooosssssssssssssssyysssss/////-`  sNm     `   .`   ``` /oooosoosyhdhysooyhd')
    print('oooooosssssssssssssshdyysssym/:::-`/.:mmo         `       :sssssyyyyyysoosyyyyy')
    print('osssssssssssssssyyhdy+++shmNs-.```.Ny``                   +ssssyyyhhyyyyyssssoo')
    print('sssshddhysyssyyyhdds/oyhsdysh-.   omm-       -.          :.+yssssyyyyysyhyyysyh')
    print('yhhhdhhhhhyyyyyhhhh/.:ysydmNh.   .hmmy      `:-`  ``    `yo-ohyyyyyyyyyyyysssss')
    print('syyyyyyyyyyhddhddmmy.`.:/++o/`  `yhhdh.     ..` ````    ohhyyyyyyyyyyyyyyyyyyss')
    print('hysyyyyhhhyhhhhhyyhhy/`  `-:. `/yyhhhyo    `.` ``      +yyyyyyyyyyyyysyssssssss')
    print('hyyhhyyhhdhhhhyyyyyyyyyo+///+syyyyyyyhy-   ..``:yo`   :hhyyyyysyyyssyysssssssss')
    print('yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyo  .-.``syy-   syyyyyyssssssssssssssssss')
    print('ssssyyyyyyyyyyyyyyyysssssosssoooooooooos-`--.`-sss.  `ssssooooooooooooooooooooo')
    print('sssyyysssssssssssssooooooooossssssssssss+-:-.`oysy   -yyyyyyyyyyyyyyyyyysyysyyy')
    print('yyyyyyyyyyyyyhhhhyhhhhhdhhdddddddhdddddd/.:-``yddy   :ddddhhdddddddddhhhhhhdhhh')
    print('hhddddddddddddddddddddddmmmmmmmdddmmdddh/.:.../hd+   .ddddddddddddhhhyhhhhhhhhh')
    print('hhhhhhhhhhhhdddhdddddddddhhhhhhhhdhhhhhh-.o+/--hy.    -osyhhhddddhhhhyyyyyyyyys')
    print('dddhhhhhhhhhhhhdddhdhhhhhhhhhhyyyssyo//:`-hyys:` ```.-::/+osyyysyyyyyyyyyhyyhys')
    print('hhhyyhhhhhdhddhhhhyyysosoysosooo//:----.`+yysssssossooyyysyhhhhyhhyyyyyyhhyyyyy')


if __name__ == '__main__':
    main()
