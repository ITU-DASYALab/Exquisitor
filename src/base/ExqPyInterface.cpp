//
// Created by Omar Shahbaz Khan on 11/05/2020.
//

#include "ExqPyInterface.h"
#include "../ExqDataHandlerH5.h"
#include "../ExqFunctionsR64.h"

using namespace exq;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::cout;
using std::endl;

PyObject* exq::initialize_py(PyObject* self, PyObject* args) {
    _pyExqV1 = PyExquisitorV1();
    vector<vector<string>> compCnfgFiles = vector<vector<string>>();
    vector<int> modFeatureDimensions;

    PyObject* compCnfgFilesPy;
    PyObject* modFeatureDimensionsPy;
    PyObject* modFunctionPy;
    PyObject* itemMetadataPy;
    PyObject* videoMetadataPy;

    // Arguments
    int iota = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
    int noms = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
    int numWorkers = (int)PyLong_AsLong(PyTuple_GetItem(args, 2));
    int segments = (int)PyLong_AsLong(PyTuple_GetItem(args, 3));
    int numModalities = (int)PyLong_AsLong(PyTuple_GetItem(args, 4));
    modFeatureDimensions = vector<int>(numModalities);
    int bClusters = (int)PyLong_AsLong(PyTuple_GetItem(args, 5));
    compCnfgFilesPy = PyTuple_GetItem(args, 6);
    for (int i = 0; i < PyList_Size(compCnfgFilesPy); i++) {
        PyObject* cnfgFilesPy = PyList_GetItem(compCnfgFilesPy, i);
        vector<string> cnfgFiles = vector<string>(PyList_Size(cnfgFilesPy));
        for (int j = 0; j < PyList_Size(cnfgFilesPy); j++) {
             cnfgFiles.emplace_back(_PyUnicode_AsString(PyList_GetItem(cnfgFilesPy,j)));
        }
        compCnfgFiles.push_back(cnfgFiles);
    }
    modFeatureDimensionsPy = PyTuple_GetItem(args, 7);
    for (int i = 0; i < PyList_Size(modFeatureDimensionsPy); i++) {
        modFeatureDimensions.push_back((int)PyLong_AsLong(PyList_GetItem(modFeatureDimensionsPy,i)));
    }

    // Sets the ExqFunctions for each modality
    modFunctionPy = PyTuple_GetItem(args,8);
    auto functions = vector<ExqFunctions<ExqDescriptor<uint64_t, uint64_t, uint64_t>>*>(numModalities);
    for (int i = 0; i < PyList_Size(modFunctionPy); i++) {
        int func = (int) PyLong_AsLong(PyList_GetItem(modFunctionPy, i));
        switch (func) {
            case 0: {
                for (int m = 0; m < numModalities; m++)
                    functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(5, iota, 48, 16,
                                                                                     16, 1000, 1000);
                break;
            }
            case 1: {
                for (int m = 0; m < numModalities; m++)
                    functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(5, iota, 48, 16,
                                                                                     16, 281474976710655,
                                                                                     200000000000000.0, 65535,
                                                                                     65535, 50000.0);
                break;
            }
            case 2: {
                for (int m = 0; m < numModalities; m++)
                    functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(7, iota, 54, 10,
                                                                                     10, 18014398509481983,
                                                                                     10000000000000000.0, 1023,
                                                                                     1023, 1000.0);
                break;
            }
            case 3: {
                int nFeat = (int) PyLong_AsLong(PyTuple_GetItem(args, 9));
                int topShift = (int) PyLong_AsLong(PyTuple_GetItem(args, 10));
                int idsShift = (int) PyLong_AsLong(PyTuple_GetItem(args, 11));
                int ratiosShift = (int) PyLong_AsLong(PyTuple_GetItem(args, 12));
                auto topMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(args, 13));
                auto topDivisor = (double) PyFloat_AsDouble(PyTuple_GetItem(args, 14));
                auto idsMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(args, 15));
                auto ratiosMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(args, 16));
                auto ratiosDivisor = (double) PyFloat_AsDouble(PyTuple_GetItem(args, 17));
                for (int m = 0; m < numModalities; m++)
                    functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(nFeat, iota, topShift, idsShift,
                                                                                     ratiosShift,
                                                                                     topMask, topDivisor, idsMask,
                                                                                     ratiosMask,
                                                                                     ratiosDivisor);
                break;
            }
            default: {
                cout << "UNKNOWN ExqFunctions CASE!" << endl;
                exit(1);
            }
        }
    }
    //TODO: Could also make this an argument choice per modality to select eCP or H5 or something different.
    auto dataHandler = new ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>(compCnfgFiles, numModalities);
    vector<ExqClassifier*> classifiers = vector<ExqClassifier*>(numModalities);
    for (int m = 0; m < numModalities; m++) {
        classifiers[m] = new ExqClassifier(modFeatureDimensions[m]);
    }
    auto worker = new ExqWorker<ExqDescriptor<uint64_t,uint64_t,uint64_t>>();

    int n_collections = PyLong_AsLong(PyTuple_GetItem(args, 9));
    itemMetadataPy = PyTuple_GetItem(args, 10);
    videoMetadataPy = PyTuple_GetItem(args, 11);
    auto itemProps = ExqArray<ItemProperties>(PyList_Size(itemMetadataPy));
    //TODO: for i->itemProps.size(); itemProps[i] = {...};
    auto vidProps = map<uint8_t,ExqArray<Props>>();
    //TODO: for i-> n_collections; vidProps.add(i, ExqArray<Props>(PyList_Size(PyList_GetItem(videoMetadataPy,i))));

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
            classifiers,
            worker,
            itemProps,
            vidProps
            );

    Py_IncRef(Py_None);
    return Py_None;
}

PyObject* exq::train_py(PyObject* self, PyObject* args) {
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

PyObject* exq::suggest_py(PyObject* self, PyObject* args) {
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
        PyList_SetItem(totalPy, i, PyLong_FromUnsignedLong(top.totalItemsConsideredPerSegment[i]));
        PyList_SetItem(workerTimesPy, i, PyLong_FromUnsignedLong(top.totalTimePerSegment[i]));
    }

    PyTuple_SetItem(finalPy, 0, suggsPy);
    PyTuple_SetItem(finalPy, 1, totalPy);
    PyTuple_SetItem(finalPy, 2, workerTimesPy);
    PyTuple_SetItem(finalPy, 3, totalTimePy);
    PyTuple_SetItem(finalPy, 4, overheadTimePy);

    return finalPy;
}

PyObject* exq::reset_model_py(PyObject* self, PyObject* args) {
    _pyExqV1._controller->reset_model();

    Py_IncRef(Py_None);
    return Py_None;
}

PyObject* exq::safe_exit_py(PyObject* self, PyObject* args) {
    //TODO: Call terminate function
    Py_IncRef(Py_None);
    return Py_None;
}

PyMODINIT_FUNC exq::PyInit_exq(void) {
    Py_Initialize();
    return PyModule_Create(&exquisitor_definition);
}