import os
from distutils.core import setup, Extension

os.environ["CC"] = "g++"
print(os.environ["CC"])
exquisitor = Extension('exq',
                       sources=['src/ExqPyInterface.cpp', 'src/ExqController.cpp',
                                'src/ExqFunctionsR64.cpp', 'src/ExqDataHandlerH5.cpp',
                                'src/ExqClassifier.cpp', 'src/ExqWorker.cpp'],
                       include_dirs=['/Library/Frameworks/Python.framework/Versions/3.8/include/python3.8',
                                     '/usr/local/Cellar/opencv/4.2.0_1/include/opencv4',
                                     '/usr/local/Cellar/hdf5/1.10.5_1/include/hdf5'],
                       library_dirs=['/Library/Frameworks/Python.framework/Versions/3.8/lib/',
                                     '/usr/local/Cellar/hdf5/1.10.5_1/lib',
                                     '/usr/local/Cellar/opencv/4.2.0_1/lib'],
                       libraries=['python3.8', 'hdf5', 'opencv_core', 'opencv_ml'],
                       extra_compile_args=['-O3', '-Wall', '-std=c++17'])

setup(name='exq',
      version='2.0',
      description='Exquisitor',
      author='Omar Shahbaz Khan',
      ext_modules=[exquisitor])
