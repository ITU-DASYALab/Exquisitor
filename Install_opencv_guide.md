Install opencv

https://docs.opencv.org/3.4/d0/db2/tutorial_macos_install.html
https://benjs.de/opencv/2021/02/03/install-opencv-and-python-without-root-access.html

Make sure cmake is installed.

1. Download source (either from git or official site)
2. Unzip
3. Create build directory (example: build_opencv)
4. Go to build directory and install using cmake
    - Example from a setup using anaconda3 python environment
    cmake -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_INSTALL_PREFIX=~/.local \
      -D PYTHON3_EXECUTABLE=$HOME/anaconda3/envs/exq/bin/python \
      -D PYTHON_INCLUDE_DIR=$HOME/anaconda3/envs/exq/include/python3.10 \
      -D PYTHON3_NUMPY_INCLUDE_DIRS=$HOME/anaconda3/envs/exq/lib/python3.10/site-packages/numpy/core/include \
      -D PYTHON3_PACKAGES_PATH=$HOME/anaconda3/envs/exq/lib/python3.10/site-packages \
      ../opencv
5. make -j8
6. make install

