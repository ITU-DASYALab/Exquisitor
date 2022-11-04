import os
from distutils.core import setup, Extension

os.environ["CC"] = "g++"
print(os.environ["CC"])
exquisitor = Extension('exq',
                       sources=['src/base/ExqPyInterface.cpp',
                                'src/base/ExqController.cpp',
                                'src/base/ExqClassifier.cpp',
                                'src/base/ExqWorker.cpp',
                                'src/base/ExqArray.cpp',
                                'src/base/eCP/ECPCluster.cpp',
                                'src/base/eCP/ECPConfig.cpp',
                                'src/base/eCP/ECPFarthestNeighbour.cpp',
                                'src/base/eCP/ECPNearestNeighbour.cpp',
                                'src/base/eCP/ECPIndex.cpp',
                                'src/base/eCP/ECPNode.cpp',
                                'src/base/eCP/ECPTree.cpp',
                                'src/base/eCP/ECPQueryOptimisationPolicies.cpp',
                                'src/ExqFunctionsR64.cpp',
                                'src/ExqDataHandlerH5.cpp',
                                'src/ExqDataHandlerECP.cpp',
                                'src/base/Metadata/ItemFilter.cpp'],
                       include_dirs=[
                           '/Library/Frameworks/Python.framework/Versions/Current/include/python3.10',
                           '/usr/local/include/opencv4',
                           '/usr/local/Cellar/hdf5/1.12.1_1/include/hdf5'
                       ],
                       library_dirs=[
                           '/Library/Frameworks/Python.framework/Versions/Current/lib',
                           '/usr/local/Cellar/hdf5/1.12.1_1/lib',
                           '/usr/local/lib' #lib path where opencv_core and opencv_ml are
                       ],
                       libraries=['python3.10', 'hdf5', 'opencv_core', 'opencv_ml'],
                       extra_compile_args=['-O3', '-Wall', '-std=c++20',
                                           '-D_LARGE_FILE_SOURCE', '-D_FILE_OFFSET_BITS=64',
                                           # '-DDEBUG',
                                           # '-DDEBUG_EXTRA',
                                           # '-DDEBUG_INIT',
                                           # '-DDEBUG_INIT_META',
                                           # '-DDEBUG_TRAIN',
                                           # '-DDEBUG_TRAIN_EXTRA',
                                           # '-DDEBUG_SUGGEST',
                                           # '-DDEBUG_SUGGEST_EXTRA',
                                           ])

setup(name='exq',
      version='1.0',
      description='Exquisitor',
      author='Omar Shahbaz Khan',
      ext_modules=[exquisitor])
