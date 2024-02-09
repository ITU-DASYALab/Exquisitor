import os
import json
from pathlib import Path
from setuptools import Extension, setup

os.environ['CC'] = 'g++'

# Paths from environment variables
inlcude_paths = [
    str(Path(os.environ['PYTHON_INCLUDE'])),
    str(Path(os.environ['HDF5_INCLUDE'])), 
    str(Path(os.environ['OPENCV_INCLUDE'])) 
]
library_paths = [
    str(Path(os.environ['PYTHON_LIBS'])),
    str(Path(os.environ['HDF5_LIBS'])),
    str(Path(os.environ['OPENCV_LIBS']))
]

# Libraries
if os.name == 'nt':
    libraries = ['python311', 'hdf5', 'hdf5_hl', 'opencv_ml', 'opencv_core']
else:
    libraries = ['python3.10', 'hdf5_serial', 'hdf5_serial_hl', 'opencv_ml', 'opencv_core']



compile_args = []
if (os.name == 'nt'):
    compile_args = ['/O2', '/std:c++20', #, '/DDEBUG_SUGGEST']
                    '/D_LARGE_FILE_SOURCE', '/D_FILE_OFFSET_BITS=64']
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
                include_dirs=inlcude_paths,
                library_dirs=library_paths,
                libraries=libraries,
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
