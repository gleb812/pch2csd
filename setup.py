import os

from setuptools import setup

import pch2csd


def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()


setup(
    name='pch2csd',
    version=pch2csd.__version__,
    author='Gleb Rogozinsky, Mikhail Chesnokov, Eugene Cherny',
    description='Convert Clavia Nord Modular G2 patches to the Csound code.',
    long_description=read('README.md'),
    keywords='csound code-generation',
    url=pch2csd.__homepage__,
    license='MIT',
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Topic :: Utilities',
        'License :: OSI Approved :: BSD License',
        'Environment :: Console',
        'Programming Language :: Python :: 3',
        'Operating System :: OS Independent',
    ],
    packages=['pch2csd', 'tests'],
    include_package_data=True,
    platforms='any',
    install_requires=['bitarray', 'tabulate'],
    entry_points={
        'console_scripts': [
            'pch2csd = pch2csd.app:main'
        ]
    },
    options={
        'build_scripts': {
            'executable': '/usr/bin/env python3'
        }
    }
)
