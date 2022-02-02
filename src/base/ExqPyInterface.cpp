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

    int iota = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
    int noms = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
    int numWorkers = (int)PyLong_AsLong(PyTuple_GetItem(args, 2));
    int segments = (int)PyLong_AsLong(PyTuple_GetItem(args, 3));
    int numModalities = (int)PyLong_AsLong(PyTuple_GetItem(args, 4));
    vector<int> modFeatureDimensions = vector<int>(numModalities);
    int bClusters = (int)PyLong_AsLong(PyTuple_GetItem(args, 5));

    PyObject* compCnfgFilesPy = PyTuple_GetItem(args, 6);
    for (int i = 0; i < PyList_Size(compCnfgFilesPy); i++) {
        PyObject* cnfgFilesPy = PyList_GetItem(compCnfgFilesPy, i);
        vector<string> cnfgFiles = vector<string>(PyList_Size(cnfgFilesPy));
        for (int j = 0; j < PyList_Size(cnfgFilesPy); j++) {
             cnfgFiles.emplace_back(_PyUnicode_AsString(PyList_GetItem(cnfgFilesPy,j)));
        }
        compCnfgFiles.push_back(cnfgFiles);
    }

    PyObject* modFeatureDimensionsPy = PyTuple_GetItem(args, 7);
    for (int i = 0; i < PyList_Size(modFeatureDimensionsPy); i++) {
        modFeatureDimensions.push_back((int)PyLong_AsLong(PyList_GetItem(modFeatureDimensionsPy,i)));
    }

    // Sets the ExqFunctions for each modality
    auto functions = vector<ExqFunctions<ExqDescriptor<uint64_t, uint64_t, uint64_t>>*>(numModalities);
    int funcType = (int) PyLong_AsLong(PyList_GetItem(args, 8));
    //funcType: 0 = same; 1 = different;
    PyObject* funcObjs = PyTuple_GetItem(args, 9);
    //funcObj: 0 = nFeat, 1 = topShift, 2 = idsShift, 3 = ratiosShift, 4 = topMask,
    //         5 = topDivisor, 6 = idsMask, 7 = ratiosMask, 8 = ratiosDivisor
    int nFeat, topShift, idsShift, ratiosShift;
    double topDivisor, ratiosDivisor;
    uint64_t topMask, idsMask, ratiosMask;
    if (funcType) {
        nFeat = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, 0), 0));
        topShift = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, 0), 1));
        idsShift = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, 0), 2));
        ratiosShift = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, 0),3));
        topMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, 0), 4));
        topDivisor = (double) PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(funcObjs, 0), 5));
        idsMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, 0), 6));
        ratiosMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, 0), 7));
        ratiosDivisor = (double) PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(funcObjs, 0), 8));
        for (int m = 0; m < numModalities; m++) {
            functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(nFeat, iota, topShift, idsShift,
                                                                             ratiosShift,
                                                                             topMask, topDivisor, idsMask,
                                                                             ratiosMask,
                                                                             ratiosDivisor);
        }
    } else {
        for (int m = 0; m < numModalities; m++) {
            nFeat = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, m), 0));
            topShift = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, m), 1));
            idsShift = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, m), 2));
            ratiosShift = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, m),3));
            topMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, m), 4));
            topDivisor = (double) PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(funcObjs, m), 5));
            idsMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, m), 6));
            ratiosMask = (uint64_t) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(funcObjs, m), 7));
            ratiosDivisor = (double) PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(funcObjs, m), 8));
            functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(nFeat, iota, topShift, idsShift,
                                                                             ratiosShift,
                                                                             topMask, topDivisor, idsMask,
                                                                             ratiosMask,
                                                                             ratiosDivisor);
        }
    }

    //TODO: Could also make this an argument choice per modality to select eCP or H5 or something different.
    auto dataHandler = new ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>(compCnfgFiles, numModalities);
    vector<ExqClassifier*> classifiers = vector<ExqClassifier*>(numModalities);
    for (int m = 0; m < numModalities; m++) {
        classifiers[m] = new ExqClassifier(modFeatureDimensions[m]);
    }
    auto worker = new ExqWorker<ExqDescriptor<uint64_t,uint64_t,uint64_t>>();

    PyObject* itemMetadataPy = PyTuple_GetItem(args, 9);
    PyObject* tmpPropsPy;
    auto itemProps = ExqArray<ItemProperties>((int)PyList_Size(itemMetadataPy));
    for (int i = 0; i < itemProps.getSize(); i++) {
        ItemProperties it = ItemProperties();
        it.collectionId = (uint8_t) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(itemMetadataPy,i),0));
        it.vid = (bool) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(itemMetadataPy,i),1));
        it.vidId = (int) PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(itemMetadataPy,i),2));
        PyObject* stdPropsPy = PyTuple_GetItem(PyList_GetItem(itemMetadataPy,i),3);
        it.stdProps.props = ExqArray<ExqArray<uint16_t>>((int)PyList_Size(stdPropsPy));
        for (int j = 0; j < PyList_Size(stdPropsPy); j++) {
            tmpPropsPy = PyList_GetItem(stdPropsPy,j);
            auto arr = ExqArray<uint16_t>((int)PyList_Size(tmpPropsPy));
            for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
               arr.setItem((uint16_t)PyLong_AsLong(PyTuple_GetItem(tmpPropsPy,k)), k);
            }
            it.stdProps.props.setItem(arr, j);
        }
        PyObject* collPropsPy = PyTuple_GetItem(PyList_GetItem(itemMetadataPy,i),3);
        it.collProps.props = ExqArray<ExqArray<uint16_t>>((int)PyList_Size(collPropsPy));
        for (int j = 0; j < PyList_Size(collPropsPy); j++) {
            tmpPropsPy = PyList_GetItem(collPropsPy, j);
            auto arr = ExqArray<uint16_t>((int)PyList_Size(tmpPropsPy));
            for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
                arr.setItem((uint16_t)PyLong_AsLong(PyTuple_GetItem(tmpPropsPy,k)), k);
            }
            it.collProps.props.setItem(arr, j);
        }
        itemProps.setItem(it,i);
    }

    PyObject* videoMetadataPy = PyTuple_GetItem(args, 10);
    auto collVidProps = ExqArray<ExqArray<Props>>((int)PyList_Size(videoMetadataPy));
    for (int c = 0; c < PyList_Size(videoMetadataPy); c++) {
        auto cVids = ExqArray<Props>((int)PyList_Size(PyList_GetItem(videoMetadataPy,c)));
        for (int v = 0; v < cVids.getSize(); v++) {
            PyObject* vidPropsPy = PyList_GetItem(videoMetadataPy,c);
            Props vp = Props();
            vp.props = ExqArray<ExqArray<uint16_t>>((int)PyList_Size(vidPropsPy));
            for (int p = 0; p < vp.props.getSize(); p++) {
                tmpPropsPy = PyList_GetItem(PyList_GetItem(vidPropsPy, v), p);
                auto arr = ExqArray<uint16_t>((int) PyList_Size(tmpPropsPy));
                for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
                    arr.setItem((uint16_t) PyLong_AsLong(PyTuple_GetItem(tmpPropsPy, k)), k);
                }
                vp.props.setItem(arr,p);
            }
            cVids.setItem(vp, v);
        }
        collVidProps.setItem(cVids,c);
    }

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
            collVidProps
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

//case 0: {
//for (int m = 0; m < numModalities; m++)
//functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(5, iota, 48, 16,
//                                                                 16, 1000, 1000);
//break;
//}
//case 1: {
//for (int m = 0; m < numModalities; m++)
//functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(5, iota, 48, 16,
//16, 281474976710655,
//200000000000000.0, 65535,
//65535, 50000.0);
//break;
//}
//case 2: {
//for (int m = 0; m < numModalities; m++)
//functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(7, iota, 54, 10,
//10, 18014398509481983,
//10000000000000000.0, 1023,
//1023, 1000.0);
//break;
//}