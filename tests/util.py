import os
import string


def get_test_resource(path: str) -> str:
    return os.path.join(os.path.dirname(__file__), 'resources', path)


def cmp_str_lines(s1: str, s2: str) -> bool:
    s1l, s2l = s1.strip().splitlines(), s2.strip().splitlines()
    if len(s1l) != len(s2l):
        return False
    for l1, l2 in zip(s1l, s2l):
        if l1.strip() != l2.strip():
            return False
    return True


def clean_up_string(s: str) -> str:
    return ''.join(c for c in s if c in string.printable)
