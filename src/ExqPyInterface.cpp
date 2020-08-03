//
// Created by Omar Shahbaz Khan on 11/05/2020.
//

#include "ExqPyInterface.h"
#include "ExqDataHandlerH5.h"
#include "ExqFunctionsR64.h"

using namespace exq;

PyObject* initialize_py(PyObject* self, PyObject* args) {
    vector<vector<string>> compCnfgFiles = vector<vector<string>>();
    int iota, noms, numWorkers, segments, numModalities, bClusters;
    vector<int> modFeatureDimensions = vector<int>();

    PyObject* compCnfgFilesPy;
    PyObject* modFeatureDimensionsPy;

    Py_Initialize();

    iota = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
    noms = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
    numWorkers = (int)PyLong_AsLong(PyTuple_GetItem(args, 2));
    segments = (int)PyLong_AsLong(PyTuple_GetItem(args, 3));
    numModalities = (int)PyLong_AsLong(PyTuple_GetItem(args, 4));
    bClusters = (int)PyLong_AsLong(PyTuple_GetItem(args, 5));
    compCnfgFilesPy = PyTuple_GetItem(args, 6);
    for (int i = 0; i < PyList_Size(compCnfgFilesPy); i++) {
        PyObject* cnfgFilesPy = PyList_GetItem(compCnfgFilesPy, i);
        vector<string> cnfgFiles = vector<string>();
        for (int j = 0; j < PyList_Size(cnfgFilesPy); j++) {
             cnfgFiles.push_back(_PyUnicode_AsString(PyList_GetItem(cnfgFilesPy,j)));
        }
        compCnfgFiles.push_back(cnfgFiles);
    }
    modFeatureDimensionsPy = PyTuple_GetItem(args, 7);
    for (int i = 0; i < PyList_Size(modFeatureDimensionsPy); i++) {
        modFeatureDimensions.push_back((int)PyLong_AsLong(PyList_GetItem(modFeatureDimensionsPy,i)));
    }

    auto dataHandler = new ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>(compCnfgFiles, numModalities);
    auto functions = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(5, 48, 16, 16, 1000, 1000);
    auto classifier = new ExqClassifier();
    const auto worker = new ExqWorker();

    _pyExqV1._controller = new ExqController<ExqDescriptor<uint64_t, uint64_t, uint64_t>>(
            iota,
            noms,
            numWorkers,
            segments,
            numModalities,
            modFeatureDimensions,
            bClusters,
            functions,
            dataHandler,
            classifier,
            worker
            );

    Py_IncRef(Py_None);
    return Py_None;
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