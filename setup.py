import os
from pathlib import Path
import sys
from setuptools import Extension, setup

os.environ['CC'] = 'g++'
vcpkg_libs = os.path.join(
                os.path.join(
                    os.path.join(
                        os.environ['VCPKG_ROOT'], 
                    'installed'), 
                os.environ['VCPKG_TRIPLET']), 
            'lib')
vcpkg_include = os.path.join(
                    os.path.join(
                        os.path.join(
                            os.environ['VCPKG_ROOT'], 
                        'installed'), 
                    os.environ['VCPKG_TRIPLET']), 
                'include')
py_libs = os.path.join(sys.exec_prefix, 'libs')
py_include = os.path.join(sys.exec_prefix,'include')
# hdf5_libs = os.path.join(os.environ['HDF5_ROOT'], 'lib')
# hdf5_include = os.path.join(os.environ['HDF5_ROOT'], 'include')
# opencv_libs = os.environ['OPENCV_LIB']
# opencv_include = os.environ['OPENCV_INCLUDE']

# print(os.environ['CC'])
# print(vcpkg_include)
# print(vcpkg_libs)
# print(hdf5_libs)
# print(hdf5_include)
# print(opencv_libs)
# print(opencv_include)

compile_args = []
if (os.name == 'nt'):
    compile_args = ['/O2', '/std:c++20']
else:
    compile_args = ['-O3', '-Wall', '-std=c++20']

sources = [
    str(Path('src/exquisitor/ExqPyInterface.cpp')),
    str(Path('src/exquisitor/ExqController.cpp')),
    str(Path('src/exquisitor/ExqClassifier.cpp')),
    str(Path('src/exquisitor/ExqWorker.cpp')),
    str(Path('src/exquisitor/eCP/ECPCluster.cpp')),
    str(Path('src/exquisitor/eCP/ECPConfig.cpp')),
    str(Path('src/exquisitor/eCP/ECPFarthestNeighbour.cpp')),
    str(Path('src/exquisitor/eCP/ECPNearestNeighbour.cpp')),
    str(Path('src/exquisitor/eCP/ECPIndex.cpp')),
    str(Path('src/exquisitor/eCP/ECPNode.cpp')),
    str(Path('src/exquisitor/eCP/ECPTree.cpp')),
    str(Path('src/exquisitor/eCP/ECPQueryOptimisationPolicies.cpp')),
    str(Path('src/exquisitor/r64/ExqFunctionsR64.cpp')),
    str(Path('src/exquisitor/r64/ExqDataHandlerECP.cpp')),
    str(Path('src/exquisitor/Metadata/ItemFilter.cpp'))
]

exquisitor = Extension(
                'exq',
                sources=sources,
                include_dirs=[
                    py_include,
                    # hdf5_include,
                    # opencv_include
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
                    # hdf5_libs,
                    # opencv_libs
                    vcpkg_libs,
                    # '/Library/Frameworks/Python.framework/Versions/Current/lib', # Mac
                    # '/usr/local/Cellar/hdf5/1.12.1_1/lib', # Mac/Linux
                    # '/usr/local/lib' # Mac/Linux, lib path where opencv_core and opencv_ml are
                    # 'C:/Program Files/WindowsApps/PythonSoftwareFoundation.Python3.10_1.10.2544.0_x64__qbz5n2kfra8p0/lib', # Windows
                    # 'C:/opencv/sources/build/lib', # Windows
                    # 'C:/Program Files/HDF_Group/HDF5/1.14.0/lib', # Windows
                ],
                libraries=['python311', 'hdf5', 'hdf5_hl', 'opencv_core4', 'opencv_ml4'],
                extra_compile_args=compile_args,
                # [
                    # '/O2', '/std:c++20', # Windows
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
                language='c++'
                )


setup(
    ext_modules=[exquisitor]
)
