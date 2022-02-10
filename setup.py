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
                       include_dirs=['/Library/Frameworks/Python.framework/Versions/Current/include/python3.10',
                                     '/usr/local/Cellar/opencv/4.2.0_1/include/opencv4',
                                     '/usr/local/Cellar/hdf5/1.10.5_1/include/hdf5'],
                       library_dirs=['/Library/Frameworks/Python.framework/Versions/Current/lib',
                                     '/usr/local/Cellar/hdf5/1.10.5_1/lib',
                                     '/usr/local/Cellar/opencv/4.2.0_1/lib'],
                       libraries=['python3.10', 'hdf5', 'opencv_core', 'opencv_ml'],
                       extra_compile_args=['-O3', '-Wall', '-std=c++17',])
                                           #'-DDEBUG_TRAIN', '-DDEBUG_TRAIN_EXTRA'])

setup(name='exq',
      version='1.0',
      description='Exquisitor',
      author='Omar Shahbaz Khan',
      ext_modules=[exquisitor])
