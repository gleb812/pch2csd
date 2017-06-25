import os


def get_test_resource(path: str) -> str:
    return os.path.join(os.path.dirname(__file__), 'resources', path)
