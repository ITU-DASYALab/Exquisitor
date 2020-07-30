//
// Created by Omar Shahbaz Khan on 11/05/2020.
//

#include "ExqPyInterface.h"

using namespace exq;

PyObject* initialize_py(PyObject* self, PyObject* args) {

}

PyObject* train_py(PyObject* self, PyObject* args) {

}

PyObject* suggest_py(PyObject* self, PyObject* args) {

}

PyObject* reset_model_py(PyObject* self, PyObject* args) {

}

PyObject* safe_exit_py(PyObject* self, PyObject* args) {

}

PyMODINIT_FUNC PyInit_exquisitor(void) {
    Py_Initialize();
    return PyModule_Create(&exquisitor_definition);
}