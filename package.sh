#!/usr/bin/env bash
rm -rf build dist pch2csd_package
mkdir pch2csd_package
cd pch2csd_package
cp -r ../* .
python setup.py sdist bdist_wheel
# twine upload dist/*