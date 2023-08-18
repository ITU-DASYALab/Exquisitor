import os
import sys
from setuptools import Extension, setup

os.environ['CC'] = 'g++'
vcpkg_libs = os.environ['VCPKG_ROOT'] + '/installed/' + os.environ['VCPKG_TRIPLET'] + '/lib'
vcpkg_include = os.environ['VCPKG_ROOT'] + '/installed/' + os.environ['VCPKG_TRIPLET'] + '/include'
py_libs = sys.exec_prefix + '/libs'
py_include = sys.exec_prefix + '/include'
compile_args = []
if (os.name == 'nt'):
    compile_args = ['/O2', '/std:c++20']
else:
    compile_args = ['-O3', '-Wall', '-std=c++20']

print(os.environ['CC'])
print(vcpkg_include)
print(vcpkg_libs)
exquisitor = Extension(
                name='exq',
                sources=[
                    'src/base/ExqPyInterface.cpp',
                    'src/base/ExqController.cpp',
                    'src/base/ExqClassifier.cpp',
                    'src/base/ExqWorker.cpp',
                    'src/base/eCP/ECPCluster.cpp',
                    'src/base/eCP/ECPConfig.cpp',
                    'src/base/eCP/ECPFarthestNeighbour.cpp',
                    'src/base/eCP/ECPNearestNeighbour.cpp',
                    'src/base/eCP/ECPIndex.cpp',
                    'src/base/eCP/ECPNode.cpp',
                    'src/base/eCP/ECPTree.cpp',
                    'src/base/eCP/ECPQueryOptimisationPolicies.cpp',
                    'src/base/r64/ExqFunctionsR64.cpp',
                    'src/base/r64/ExqDataHandlerECP.cpp',
                    'src/base/Metadata/ItemFilter.cpp'
                ],
                include_dirs=[
                    py_include,
                    vcpkg_include
                    # '/Library/Frameworks/Python.framework/Versions/Current/include/python3.10', # Mac
                    # '/usr/local/include/opencv4', # Mac/Linux
                    # '/usr/local/Cellar/hdf5/1.12.1_1/include/hdf5' # Mac/Linux
                    # 'C:/Program Files/WindowsApps/PythonSoftwareFoundation.Python3.10_1.10.2544.0_x64__qbz5n2kfra8p0/include', # Windows
                    # 'C:/opencv/sources/build/install/include', # Windows
                    # 'C:/Program Files/HDF_Group/HDF5/1.14.0/include' # Windows
                ],
                library_dirs=[
                    py_libs,
                    vcpkg_libs
                    # '/Library/Frameworks/Python.framework/Versions/Current/lib', # Mac
                    # '/usr/local/Cellar/hdf5/1.12.1_1/lib', # Mac/Linux
                    # '/usr/local/lib' # Mac/Linux, lib path where opencv_core and opencv_ml are
                    # 'C:/Program Files/WindowsApps/PythonSoftwareFoundation.Python3.10_1.10.2544.0_x64__qbz5n2kfra8p0/lib', # Windows
                    # 'C:/opencv/sources/build/lib', # Windows
                    # 'C:/Program Files/HDF_Group/HDF5/1.14.0/lib', # Windows
                ],
                libraries=['python311', 'hdf5', 'hdf5_hl', 'opencv_core4', 'opencv_ml4'],
                extra_compile_args=compile_args
                # [
                #     '/O2', '/std:c++20', # Windows
                    # '-O3', '-Wall', '-std=c++20', # Linux
                    # '-D_LARGE_FILE_SOURCE', '-D_FILE_OFFSET_BITS=64',
                    # '-DDEBUG',
                    # '-DDEBUG_EXTRA',
                    # '-DDEBUG_INIT',
                    # '-DDEBUG_INIT_META',
                    # '-DDEBUG_TRAIN',
                    # '-DDEBUG_TRAIN_EXTRA',
                    # '-DDEBUG_SUGGEST',
                    # '-DDEBUG_SUGGEST_EXTRA',
                #]
                )

setup(name='exq',
      version='1.0',
      description='Exquisitor',
      author='Omar Shahbaz Khan',
      ext_modules=[exquisitor])
