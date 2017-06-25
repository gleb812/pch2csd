import json
import os
from typing import Dict, Any


def get_template_path(name: str) -> str:
    return os.path.join(os.path.dirname(__file__), 'templates', f'{name}.txt')


def get_template_module_path(name: str) -> str:
    return os.path.join(os.path.dirname(__file__), 'templates', 'modules', f'{name}.txt')


def _read_json(filename: str) -> Dict[str, Any]:
    path = os.path.join(os.path.dirname(__file__), filename)
    with open(path, 'r') as f:
        return json.load(f)


class ProjectData:
    def __init__(self):
        self._value_maps = None
        self._mod_type_connection = None
        self._mod_type_name = None

    @property
    def value_maps(self):
        if self._value_maps is None:
            self._value_maps = _read_json('value_maps.json')
        return self._value_maps

    @property
    def mod_type_connection(self):
        if self._mod_type_connection is None:
            self._mod_type_connection = {int(k): v for k, v
                                         in _read_json('mod_type_connections.json').items()}
        return self._mod_type_connection

    @property
    def mod_type_name(self):
        if self._mod_type_name is None:
            self._mod_type_name = {int(k): v for k, v
                                   in _read_json('mod_type_name.json').items()
                                   if v != 'Unknown'}
        return self._mod_type_name
