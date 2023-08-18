# Exquisitor

Exquisitor is a large scale multimodal interactive learning approach.
It uses an approximate high-dimensional index with a linear SVM classifier to facilitate rapid interactive feedback rounds
for large multimedia collections using modest computing resources.


```
@inproceedings{khan2020interactive,
  title={Interactive learning for multimedia at large},
  author={Khan, Omar Shahbaz and J{\'o}nsson, Bj{\"o}rn {\TH}{\'o}r and Rudinac, Stevan and Zah{\'a}lka, Jan and Ragnarsd{\'o}ttir, Hanna and {\TH}orleiksd{\'o}ttir, {\TH}{\'o}rhildur and Gu{\dh}mundsson, Gylfi {\TH}{\'o}r and Amsaleg, Laurent and Worring, Marcel},
  booktitle={European Conference on Information Retrieval},
  year={2020},
}
```

## Requirements

* python 3.11 or higher
* cmake 3.27 or higher
* opencv 4.5.5 or higher
* hdf5 1.14.0 or higher

### Windows

* For development on Windows we have opted to use MSVC Visual C++ 14 or higher along with vcpkg. 
Using vcpkg install hdf5 and opencv (may need to install opencv using --host-triplet=x64-windows-static).

# Installation

Set the following environment variables:
  * VCPKG_ROOT='C:\vcpkg'
  * VCPKG_TRIPLET='x64-windows'
  Examples for Windows

Run the following commands:
```
python -m build
pip install --force-reinstall dist/*.whl
```


## Old setup.py installation
* Update include and library directory paths in setup.py
* Add library directory for opencv and hdf5 to $LD_LIBRARY_PATH (Linux) or $DYLD_LIBRARY_PATH (MacOS). Example for .bashrc:
    ```
    export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
    or
    export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
    ```
* python3 setup.py install
* Test if everything is working by running the three test script with argument 0. (example: python3 test_initialize.py 0)
* If you encounter error relating to unavailable function for macOS use the following command to set the target macOS version to 12:
  ```
  export MACOSX_DEPLOYMENT_TARGET=12.0
  ```
  All features should be available for macOS 10.15 and above.

Note: If you are using CLion set X_LIBRARY_PATH under Environment Variable in the run configuration
