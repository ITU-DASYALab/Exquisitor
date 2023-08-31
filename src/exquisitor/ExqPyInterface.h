//
// Created by Omar Shahbaz Khan on 11/05/2020.
//

#ifndef EXQUISITOR_EXQPYINTERFACE_H
#define EXQUISITOR_EXQPYINTERFACE_H

#include <Python.h>
#include "ExqController.h"

namespace exq {

    struct PyExquisitorV1 {
        ExqController<uint64_t>* _controller;
    };
    PyExquisitorV1 _pyExqV1;


    static PyObject* initialize_py([[maybe_unused]] PyObject* self, PyObject* args);
    static PyObject* train_py([[maybe_unused]] PyObject* self, PyObject* args);
    static PyObject* suggest_py([[maybe_unused]] PyObject* self, PyObject* args);
    static PyObject* reset_model_py([[maybe_unused]] PyObject* self, [[maybe_unused]] PyObject* args);
    static PyObject* safe_exit_py([[maybe_unused]] PyObject* self, [[maybe_unused]] PyObject* args);
    static PyObject* get_descriptors_info_py([[maybe_unused]] PyObject* self, PyObject* args);

    static PyMethodDef exq_methods[] = {
            {"initialize", initialize_py, METH_VARARGS, "Initialize exquisitor"},
            {"train", train_py, METH_VARARGS, "Train the classifier (linear SVM)"},
            {"suggest", suggest_py, METH_VARARGS, "Get suggestions from current model"},
            {"reset_model", reset_model_py, METH_VARARGS, "Reset the model"},
            {"safe_close", safe_exit_py, METH_NOARGS, "Safely free up memory"},
            {"get_descriptors_info", get_descriptors_info_py, METH_VARARGS, "Get descriptor information from item ids list for a"
                                                                 "specific modality"},
            {NULL, NULL, 0, NULL}
    };

    static struct PyModuleDef exq_definition = {
            PyModuleDef_HEAD_INIT,
            "exq",
            "A Python module that executes Exquisitor functions.",
            -1,
            exq_methods,
    };

    PyMODINIT_FUNC PyInit_exq(void);
}

#endif //EXQUISITOR_EXQPYINTERFACE_H
