#!/bin/bash
# MAC (brew)
# export PYTHON_INLCUDE="/opt/homebrew/Cellar/python3.11/..."
# export PYTHON_LIBS="/opt/homebrew/Cellar/python3.11/..."
# export HDF5_INCLUDE="/opt/homebrew/include"
# export HDF5_LIBS="/opt/homebrew/lib"
# export OPENCV_INCLUDE="/opt/homebrew/include"
# export OPENCV_LIBS="/opt/homebrew/lib"

# Ubuntu (WSL)
export PYTHON_INCLUDE="/usr/include/python3.10/"
export PYTHON_LIBS="/usr/lib/x86_64-linux-gnu/"
export HDF5_INCLUDE="/usr/include/hdf5/serial/"
export HDF5_LIBS="/usr/lib/x86_64-linux-gnu/"
export OPENCV_INCLUDE="/usr/include/opencv4/"
export OPENCV_LIBS="/usr/lib/x86_64-linux-gnu/"
python3 -m build
python3 -m pip install --force-reinstall dist/*.whl

# In case the above fails run the following instead, by removing the #.
# Remember to add # in front of the above pip install statement if running this.
# python -m delvewheel repair dist\exq.whl
# python -m pip install --force-reinstall wheelhouse\exq.whl 