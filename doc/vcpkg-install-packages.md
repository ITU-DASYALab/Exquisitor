# Windows with MinGW (ucrt64)
vcpkg install opencv:x64-mingw-dynamic --host-triplet=x64-mingw-dynamic
vcpkg install hdf5:x64-mingw-dynamic --host-triplet=x64-mingw-dynamic

OpenCV may require adding "-DWITH_OBSENSOR=OFF" in the portsfile.cmake under opencv4