import argparse
import os
import sys
import json   # added 02012026
from io import StringIO

from tabulate import tabulate

from . import __version__, __homepage__
from .csdgen import ZakSpace, Csd
from .parse import parse_pch2
from .patch import Patch, Location, CableColor, CableType, ModuleParameters, Module, Cable
from .resources import get_template_module_path, ProjectData
from .udo import UdoTemplate, UdoTemplateValidation
from .util import get_test_resource


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

def validate_udo_from_file(filename: str, io=sys.stdout, print_action=True):
    if not os.path.isfile(filename):
        print(f"error: file '{filename}' not found", file=io)
        return False
    
    if filename.lower().endswith('.pch2'):
        if print_action:
            print(f"Checking all modules in PCH2 file: {filename}", file=io)
        
        data = ProjectData()
        path = os.path.abspath(os.path.expanduser(filename))
        patch = parse_pch2(data, path)
        
    elif filename.lower().endswith('.json'):
        if print_action:
            print(f"Checking all modules in JSON file: {filename}", file=io)
        
        try:
            json_data = load_json_patch(filename)
            patch = json_to_patch(json_data, filename)
        except Exception as e:
            print(f"error: failed to load JSON file: {e}", file=io)
            return False
    else:
        print("error: file should have extension '.pch2' or '.json'", file=io)
        return False
    
    if not patch.modules:
        print("error: file contains no modules", file=io)
        return False
    
    unique_types = set(m.type for m in patch.modules)
    all_valid = True
    
    for type_id in sorted(unique_types):
        module_name = next((m.type_name for m in patch.modules if m.type == type_id), f"Unknown_{type_id}")
        
        if print_action:
            print(f"\nChecking module type '{type_id}' ({type_id}.txt) - {module_name}:", file=io)
        
        result = validate_udo(type_id, io, print_action=False)
        if not result:
            all_valid = False
    
    return all_valid



def print_pch2(fn: str):  # modified 08012026
    if fn.lower().endswith('.pch2'):
        if not os.path.isfile(fn):
            print(f"error: file '{fn}' not found")
            exit(-1)

        data = ProjectData()
        path = os.path.abspath(os.path.expanduser(fn))
        patch = parse_pch2(data, path)
        _print_patch_content(patch, os.path.basename(path))

    elif fn.lower().endswith('.json'):
        if not os.path.isfile(fn):
            print(f"error: file '{fn}' not found")
            exit(-1)
            
        try:
            json_data = load_json_patch(fn)
            patch = json_to_patch(json_data, fn)
            _print_patch_content(patch, os.path.basename(fn))
        except Exception as e:
            print(f"error: failed to load JSON file: {e}")
            exit(-1)
    else:
        print("error: file should have extension '.pch2' or '.json'")
        exit(-1)

def _print_patch_content(patch: Patch, filename: str):
    mod_table = [['Name', 'ID', 'Type', 'Parameters', 'Modes', 'Area', 'Hpos', 'Vpos']]
    for m in patch.modules:
        p = patch.find_mod_params(m.location, m.id)
        mod_table.append([m.type_name,
                          m.id,
                          m.type,
                          str(p.values),
                          str(m.modes),
                          m.location.short_str(),
                          m.hpos,
                          m.vpos])
    
    cab_table = [['From', '', 'To', 'Color', 'Type', 'Area']]
    for c in patch.cables:
        mf_name = "Unknown"
        mt_name = "Unknown"
        
        mf = patch.find_module(c.module_from, c.loc)
        mt = patch.find_module(c.module_to, c.loc)
        
        if mf:
            mf_name = mf.type_name
        if mt:
            mt_name = mt.type_name
            
        pin1, pin2 = c.type.short_str().split('-')
        cab_table.append([
            '{}(id={}, {}={})'.format(mf_name, c.module_from, pin1, c.jack_from),
            '->',
            '{}(id={}, {}={})'.format(mt_name, c.module_to, pin2, c.jack_to),
            c.color.short_str(),
            c.type.short_str(),
            c.loc.short_str()])
    
    print('Patch file: {}\n'.format(filename))
    print('Modules')
    print(tabulate(mod_table, headers='firstrow', tablefmt='simple'))
    print('\nCables')
    print(tabulate(cab_table, headers='firstrow', tablefmt='simple'))

def convert_pch2(fn: str):
    if not os.path.isfile(fn):
        print(f"error: file '{fn}' not found")
        exit(-1)
    
    full_path = os.path.abspath(os.path.expanduser(fn))
    
    if fn.lower().endswith('.pch2'):
        data = ProjectData()
        p = parse_pch2(data, full_path)
        
    elif fn.lower().endswith('.json'):
        try:
            json_data = load_json_patch(full_path)
            p = json_to_patch(json_data, full_path)
        except Exception as e:
            print(f"error: failed to load JSON file: {e}")
            exit(-1)
    else:
        print("error: file should have extension '.pch2' or '.json'")
        exit(-1)
    
    zak = ZakSpace()
    try:
        udos = zak.connect_patch(p)
    except ValueError as e:
        print('error: {}'.format(e))
        exit(-1)
    
    csd = Csd(p, zak, udos)
    dirname = os.path.dirname(full_path)
    csd_save_path = os.path.join(dirname, os.path.basename(fn) + '.csd')
    
    with open(csd_save_path, 'w') as f:
        f.write(csd.get_code())
    
    return csd_save_path

def gen_udo_status_doc():
    tpl_url = 'https://github.com/gleb812/pch2csd/blob/master/pch2csd/resources/templates/modules/{}.txt'
    data = ProjectData()
    with open('Module-implementation-status.md', 'w') as md:
        md.write('> **NB!** This file is automatically generated.\n\n')
        md.write('| Template | Module name | Status |\n')
        md.write('|----------|-------------|--------|\n')

        for p in [parse_pch2(data, get_test_resource(pch2file))
                  for pch2file in ['test_all_modules_1.pch2',
                                   'test_all_modules_2.pch2']]:
            for m in p.modules:
                status = StringIO()
                validate_udo(m.type, status, print_action=False)
                md.write('| [`{}`]({}) | `{}` | ```{}``` |\n'.format(
                    '{}.txt'.format(m.type),
                    tpl_url.format(m.type),
                    m.type_name,
                    '```<br>```'.join(status.getvalue().splitlines())))

def load_json_patch(fn: str): # added 08012026
    with open(fn, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    if 'modules' not in data:
        print("error: invalid JSON patch file (missing 'modules')")
        exit(-1)
    
    return data

def json_to_patch(json_data, filename: str) -> Patch:
    data = ProjectData()
    patch = Patch(data)
    patch.textpad = json_data.get("textpad", "")
    
    for m in json_data["modules"]:
        area = m.get("area", "VOICE")
        try:
            loc = Location.from_str(area)
        except:
            loc = Location.VOICE_AREA 
        
        type_id = m.get("type", 0)
        
        if type_id == 0:
            type_name = m.get("name", "")
            for k, v in data.mod_type_name.items():
                if v == type_name:
                    type_id = k
                    break
        
        mod = Module(data, loc, type_id, m["id"], 
                     m.get("modes", []), 
                     m.get("name", None),
                     m.get("hpos", None),
                     m.get("vpos", None))
        patch.modules.append(mod)
        
        params_str = m.get("parameters", "")
        if params_str and params_str != "None":
            try:
                params_str = str(params_str).strip("[]")
                if params_str:
                    params = [int(p.strip()) for p in params_str.split(",") if p.strip()]
                    if params:
                        mod_params = ModuleParameters(loc, m["id"], len(params), params)
                        patch.mod_params.append(mod_params)
            except Exception as e:
                print(f"Warning: Could not parse parameters for module {m['id']}: {e}")
    
    for c in json_data["cables"]:
        area = c.get("area", "VOICE")
        try:
            loc = Location.from_str(area)
        except:
            loc = Location.VOICE_AREA
        
        color_str = c.get("color", "red").lower()
        color_map = {
            "red": CableColor.RED,
            "blue": CableColor.BLUE,
            "yellow": CableColor.YELLOW,
            "orange": CableColor.ORANGE,
            "green": CableColor.GREEN,
            "purple": CableColor.PURPLE,
            "white": CableColor.WHITE
        }
        color = color_map.get(color_str, CableColor.RED)
        
        type_str = c.get("type", "out-in").lower()
        if "in-in" in type_str:
            cable_type = CableType.IN_TO_IN
        else:
            cable_type = CableType.OUT_TO_IN
        
        if "from" in c and "to" in c:
            from_info = c["from"]
            to_info = c["to"]
            
            module_from = from_info.get("id", 0)
            jack_from = from_info.get("jack", 0)
            module_to = to_info.get("id", 0)
            jack_to = to_info.get("jack", 0)
            
            cable = Cable(
                loc, 
                cable_type,
                color,
                module_from,
                jack_from,
                module_to,
                jack_to
            )
            patch.cables.append(cable)
        else:
            module_from = c.get("module_from", 0)
            jack_from = c.get("jack_from", 0)
            module_to = c.get("module_to", 0)
            jack_to = c.get("jack_to", 0)
            
            cable = Cable(
                loc, 
                cable_type,
                color,
                module_from,
                jack_from,
                module_to,
                jack_to
            )
            patch.cables.append(cable)
    
    return patch

def export_to_json(patch: Patch, filename: str):    # added 08012026
    patch_data = {
        "filename": os.path.basename(filename),
        "modules": [],
        "cables": [],
        "textpad": patch.textpad if patch.textpad else ""
    }
    
    for m in patch.modules:
        p = patch.find_mod_params(m.location, m.id)
        module_info = {
            "name": m.type_name,
            "id": m.id,
            "type": m.type,
            "parameters": str(p.values) if p else None,
            "modes": m.modes,
            "area": m.location.short_str(),
            "hpos": m.hpos,
            "vpos": m.vpos
        }
        patch_data["modules"].append(module_info)
    
    for c in patch.cables:
        mf = patch.find_module(c.module_from, c.loc)
        mt = patch.find_module(c.module_to, c.loc)
        pin1, pin2 = c.type.short_str().split('-')
        cable_info = {
            "from": {
                "module": mf.type_name if mf else "Unknown",
                "id": c.module_from,
                "jack": c.jack_from,
                "pin": pin1
            },
            "to": {
                "module": mt.type_name if mt else "Unknown",
                "id": c.module_to,
                "jack": c.jack_to,
                "pin": pin2
            },
            "color": c.color.short_str(),
            "type": c.type.short_str(),
            "area": c.loc.short_str()
        }
        patch_data["cables"].append(cable_info)
    
    json_filename = os.path.splitext(filename)[0] + ".json"
    with open(json_filename, 'w', encoding='utf-8') as f:
        json.dump(patch_data, f, indent=2, ensure_ascii=False)
    
    return json_filename

def main():
    arg_parser = argparse.ArgumentParser(
        prog='pch2csd',
        description='convert Clavia Nord Modular G2 patches to the Csound code',
        epilog='Version {}, homepage: {}'.format(__version__, __homepage__))

    arg_parser.add_argument('arg', metavar='arg', nargs='?', default=None,
                            help='a pch2 or JSON file path or an UDO numerical ID')
    arg_parser.add_argument('-d', '--debug', action='store_const', const=True,
                            help='print a stack trace in case of error')
    group = arg_parser.add_mutually_exclusive_group()
    group.add_argument('-p', '--print', action='store_const', const=True,
                       help='parse the patch file and print its content')
    group.add_argument('-j', '--json', action='store_const', const=True, 
                       help='export patch data to JSON file')
    group.add_argument('-c', '--check-udo', action='store_const', const=True,
                       help="validate the UDO template file (overrides '-p')")
    group.add_argument('-v', '--version', action='version',
                       version='%(prog)s ' + __version__)
    group.add_argument('-e', action='store_const', const=True,
                       help='show the elephant and exit')

    args = arg_parser.parse_args()

    if args.arg is None:
        if args.print or args.json or args.check_udo or args.e:
            print("error: please specify a file path or UDO ID")
            arg_parser.print_help()
            exit(-1)
        else:
            arg_parser.print_help()
            exit(0)
    
    if args.check_udo:
        try:
            type_id = int(args.arg)
            validate_udo(type_id)
        except ValueError:
            validate_udo_from_file(args.arg)
    
    elif args.print:
        print_pch2(args.arg)
    
    elif args.json:
        if not args.arg.lower().endswith('.pch2'):
            print("error: for JSON export, patch file should have extension '.pch2'")
            exit(-1)
        data = ProjectData()
        path = os.path.abspath(os.path.expanduser(args.arg))
        patch = parse_pch2(data, path)
        json_file = export_to_json(patch, path)
        print(f'JSON export done, created file: {json_file}')

    elif args.e:
        show_elephant()

    else:
        if args.arg == 'gen_udo_status_doc':
            gen_udo_status_doc()
        else:
            if not os.path.isfile(args.arg):
                print(f"error: file '{args.arg}' not found")
                exit(-1)
                
            try:
                saved_csd = convert_pch2(args.arg)
                print('conversion done, created file: {}'.format(saved_csd))
            except Exception as e:
                print(f"error: {e}")
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
