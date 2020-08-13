//
// Created by Omar Shahbaz Khan on 11/05/2020.
//

#include "ExqPyInterface.h"
#include "ExqDataHandlerH5.h"
#include "ExqFunctionsR64.h"

using namespace exq;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;

PyObject* initialize_py(PyObject* self, PyObject* args) {
    vector<vector<string>> compCnfgFiles = vector<vector<string>>();
    vector<int> modFeatureDimensions = vector<int>();

    PyObject* compCnfgFilesPy;
    PyObject* modFeatureDimensionsPy;

    int iota = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
    int noms = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
    int numWorkers = (int)PyLong_AsLong(PyTuple_GetItem(args, 2));
    int segments = (int)PyLong_AsLong(PyTuple_GetItem(args, 3));
    int numModalities = (int)PyLong_AsLong(PyTuple_GetItem(args, 4));
    int bClusters = (int)PyLong_AsLong(PyTuple_GetItem(args, 5));
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
    vector<uint32_t> trainIds = vector<uint32_t>();
    vector<short> trainLabels = vector<short>();

    PyObject* trainIdsPy = PyTuple_GetItem(args, 0);
    PyObject* trainLabelsPy = PyTuple_GetItem(args, 1);
    for (int i = 0; i < PyList_Size(trainIdsPy); i++) {
        trainIds.push_back((int)PyLong_AsLong(PyList_GetItem(trainIdsPy,i)));
        trainLabels.push_back((short)PyLong_AsLong(PyList_GetItem(trainLabelsPy,i)));
    }

    auto times = _pyExqV1._controller->train(trainIds, trainLabels);

    PyObject* timeList = PyList_New(times.size());
    for (int i = 0; i < times.size(); i++) {
        PyList_SET_ITEM(timeList, i, PyFloat_FromDouble(times[i]));
    }

    return timeList;
}

PyObject* suggest_py(PyObject* self, PyObject* args) {
    TopResults top;
    PyObject* suggsPy;
    PyObject* totalPy;
    PyObject* workerTimesPy;
    PyObject* finalPy;
    PyObject* totalTimePy;
    PyObject* overheadTimePy;

    vector<uint32_t> seen = vector<uint32_t>();
    int r = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
    int segments = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
    PyObject* alreadySeenPy = PyTuple_GetItem(args, 2);

    for (int i = 0; i < PyList_Size(alreadySeenPy); i++) {
        seen.push_back((uint32_t)PyLong_AsLong(PyList_GetItem(alreadySeenPy,i)));
    }
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    top = _pyExqV1._controller->suggest(r, seen);
    time_point<high_resolution_clock> finish = high_resolution_clock::now();

    suggsPy = PyList_New(top.suggs.size());
    totalPy = PyList_New(segments);
    workerTimesPy = PyList_New(segments);
    finalPy = PyTuple_New(5);
    totalTimePy = PyFloat_FromDouble(duration<double, milli>(finish-begin).count());
    overheadTimePy = PyFloat_FromDouble(top.overheadTime);

    for (int i = 0; i < top.suggs.size(); i++) {
        PyList_SetItem(suggsPy, i, PyLong_FromUnsignedLong((unsigned long)(top.suggs[i])));
    }
    for (int i = 0; i < segments; i++) {
        PyList_SetItem(totalPy, i, PyLong_FromUnsignedLong(top.total[i]));
        PyList_SetItem(workerTimesPy, i, PyLong_FromUnsignedLong(top.time[i]));
    }

    PyTuple_SetItem(finalPy, 0, suggsPy);
    PyTuple_SetItem(finalPy, 1, totalPy);
    PyTuple_SetItem(finalPy, 2, workerTimesPy);
    PyTuple_SetItem(finalPy, 3, totalTimePy);
    PyTuple_SetItem(finalPy, 4, overheadTimePy);

    return finalPy;
}

PyObject* reset_model_py(PyObject* self, PyObject* args) {
    _pyExqV1._controller->reset_model();

    Py_IncRef(Py_None);
    return Py_None;
}

PyObject* safe_exit_py(PyObject* self, PyObject* args) {
    //TODO: Call terminate function
    Py_IncRef(Py_None);
    return Py_None;
}

PyMODINIT_FUNC PyInit_exquisitor(void) {
    Py_Initialize();
    return PyModule_Create(&exquisitor_definition);
}