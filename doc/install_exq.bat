set PYTHON_INCLUDE=C:\Python311\include
set PYTHON_LIBS=C:\Python311\libs
set HDF5_INCLUDE=C:\vcpkg\installed\x64-windows\include
set HDF5_LIBS=C:\vcpkg\installed\x64-windows\lib
set OPENCV_INCLUDE=C:\vcpkg\installed\x64-windows\include
set OPENCV_LIBS=C:\vcpkg\installed\x64-windows\lib

python -m build
for %%f in (dist\*.whl) do (
    python -m pip install --force-reinstall dist\%%~Nf.whl
)

@REM In case the above fails with "ImportError: DLL load failed while importing exq",
@REM run the following instead.
@REM Remember to add @REM in front of the above for loop statement if running this.
for %%f in (dist\*.whl) do (
    python -m delvewheel repair dist\%%~Nf.whl
    python -m pip install --force-reinstall wheelhouse\%%~Nf.whl 
)