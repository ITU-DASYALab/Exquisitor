#!/bin/bash

export PYTHON_INLCUDE="/opt/homebrew/Cellar/python3.11/..."
export PYTHON_LIBS="/opt/homebrew/Cellar/python3.11/..."
export HDF5_INCLUDE="/opt/homebrew/include"
export HDF5_LIBS="/opt/homebrew/lib"
export OPENCV_INCLUDE="/opt/homebrew/include"
export OPENCV_LIBS="/opt/homebrew/lib"

python -m build
python -m pip install --force-reinstall dist/*.whl

# In case the above fails run the following instead, by removing the #.
# Remember to add # in front of the above pip install statement if running this.
# python -m delvewheel repair dist\exq.whl
# python -m pip install --force-reinstall wheelhouse\exq.whl 