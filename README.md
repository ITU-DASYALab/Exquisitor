# Exquisitor


## Requirements

* python 3.10
* cmake 3.10 or higher
* opencv 4.5.5 (Link to installation guide)
* hdf5 10.0.2

## Compile

* Update include and library directory paths in setup.py
* Add library directory for opencv and hdf5 to $LD_LIBRARY_PATH (Linux) or $DYLD_LIBRARY_PATH (MacOS). Example for .bashrc:
    ```
    export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
    or
    export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
    ```
* python3 setup.py install
* Test if everything is working by running the three test script with argument 0. (example: python3 test_initialize.py 0)

Note: If you are using CLion set X_LIBRARY_PATH under Environment Variable in the run configuration
