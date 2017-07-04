import json
import os
from typing import Dict, Any, List


def get_template_dir() -> str:
    return os.path.join(os.path.dirname(__file__), 'templates')


def get_template_user_dir() -> str:
    return os.path.join(os.path.expanduser('~'), 'pch2csd')


def get_template_path(name: str) -> str:
    fn = '{}.txt'.format(name)
    user_tpl = os.path.join(get_template_user_dir(), fn)
    if os.path.isfile(user_tpl):
        return user_tpl
    else:
        return os.path.join(get_template_dir(), '{}.txt'.format(name))


def get_template_module_path(name: int) -> str:
    fn = '{}.txt'.format(name)
    user_tpl = os.path.join(get_template_user_dir(), 'modules', fn)
    if os.path.isfile(user_tpl):
        return user_tpl
    else:
        return os.path.join(get_template_dir(), 'modules', fn)


def _read_json(filename: str) -> Dict[str, Any]:
    path = os.path.join(os.path.dirname(__file__), filename)
    with open(path, 'r') as f:
        return json.load(f)


class ProjectData:
    def __init__(self):
        self._mod_params = None
        self._value_maps = None
        self._mod_type_connections = None
        self._mod_type_name = None
        self._new_mod_id = 1023

    @property
    def value_maps(self) -> Dict[str, List[int]]:
        if self._value_maps is None:
            self._value_maps = _read_json('value_maps.json')
        return self._value_maps

    @property
    def mod_type_connections(self) -> Dict[int, Dict[str, List[str]]]:
        if self._mod_type_connections is None:
            self._mod_type_connections = {int(k): v for k, v
                                          in _read_json('mod_type_connections.json').items()}
        return self._mod_type_connections

    @property
    def mod_type_name(self) -> Dict[int, str]:
        if self._mod_type_name is None:
            self._mod_type_name = {int(k): v for k, v
                                   in _read_json('mod_type_name.json').items()
                                   if v != 'Unknown'}
        return self._mod_type_name

    @property
    def mod_params(self) -> Dict[int, List[Any]]:
        if self._mod_params is None:
            self._mod_params = {int(k): v for k, v
                                in _read_json('mod_params.json').items()}
        return self._mod_params

    @property
    def new_mod_id(self) -> int:
        self._new_mod_id += 1
        return self._new_mod_id
