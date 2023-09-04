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

In install_exq.sh / install_exq.bat change the paths to the correct locations for Python, HDF5 and OpenCV.

Verify that the libraries for opencv libopencv_ml and libopencv_core exists. In some installations they have the name libopencv_ml4 and libopencv_core4 or libopencv_world4XX. Resolve these by either creating symbolic links to opencv_ml and opencv_core filenames or update setup.py "libraries" variable (Note: In the if-else clause 'nt' = Windows).

Run the appropriate install_exq script for your system.