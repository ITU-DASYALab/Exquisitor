//
// Created by Omar Shahbaz Khan on 11/05/2020.
//

#include "ExqPyInterface.h"
#include "../ExqDataHandlerECP.h"
#include "../ExqFunctionsR64.h"

using namespace exq;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::cout;
using std::endl;
using std::unordered_set;

PyObject* exq::initialize_py([[maybe_unused]] PyObject* self, PyObject* args) {
    _pyExqV1 = PyExquisitorV1();

    cout << "Initializing Exquisitor... " << endl;

    vector<vector<string>> compCnfgFiles = vector<vector<string>>();

    cout << "Getting Arguments... " << endl;
    int iota = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
    cout << "iota: " << iota << endl;
    int noms = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
    cout << "noms: " << noms << endl;
    int numWorkers = (int)PyLong_AsLong(PyTuple_GetItem(args, 2));
    cout << "numWorkers: " << numWorkers << endl;
    int segments = (int)PyLong_AsLong(PyTuple_GetItem(args, 3));
    cout << "segments: " << segments << endl;
    int numModalities = (int)PyLong_AsLong(PyTuple_GetItem(args, 4));
    cout << "numModalities: " << numModalities << endl;
    vector<int> modFeatureDimensions = vector<int>(numModalities);
    int bClusters = (int)PyLong_AsLong(PyTuple_GetItem(args, 5));
    cout << "bClusters: " << bClusters << endl;
    PyObject* compCnfgFilesPy = PyTuple_GetItem(args, 6);

    cout << "Reading modality feature dimensions" << endl;
    PyObject* modFeatureDimensionsPy = PyTuple_GetItem(args, 7);
    for (int i = 0; i < PyList_Size(modFeatureDimensionsPy); i++) {
        modFeatureDimensions[i] = (int) PyLong_AsLong(PyList_GetItem(modFeatureDimensionsPy,i));
        cout << "modFeatureDimensions[" << i << "]: " << modFeatureDimensions[i] << endl;
    }

    // Sets the ExqFunctions for each modality
    auto functions = vector<ExqFunctions<ExqDescriptor<uint64_t, uint64_t, uint64_t>>*>(numModalities);
    int funcType = (int) PyLong_AsLong(PyTuple_GetItem(args, 8));
    cout << "funcType: " << funcType << endl;
    //funcType: 0 = same; 1 = different;
    PyObject* funcObjsPy = PyTuple_GetItem(args, 9);
    //funcObj: 0 = nFeat, 1 = topShift, 2 = idsShift, 3 = ratiosShift, 4 = topMask,
    //         5 = topDivisor, 6 = idsMask, 7 = ratiosMask, 8 = ratiosDivisor
    int nFeat, topShift, idsShift, ratiosShift;
    double topDivisor, ratiosDivisor;
    uint64_t topMask, idsMask, ratiosMask;
    cout << "Creating functions object" << endl;
    PyObject* funcObjPy;
    if (funcType) {
        funcObjPy = PyList_GetItem(funcObjsPy,0);
        cout << "Same modality settings" << endl;
        nFeat = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 0));
        cout << "nFeat: " << nFeat << endl;
        topShift = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 1));
        cout << "topShift: " << topShift << endl;
        idsShift = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 2));
        cout << "idsShift: " << idsShift << endl;
        ratiosShift = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 3));
        cout << "ratiosShift: " << ratiosShift << endl;
        topMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 4));
        cout << "topMask: " << topMask << endl;
        topDivisor = (double) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 5));
        cout << "topDivisor: " << topDivisor << endl;
        idsMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 6));
        cout << "idsMask: " << idsMask << endl;
        ratiosMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 7));
        cout << "ratiosMask: " << ratiosMask << endl;
        ratiosDivisor = (double) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 8));
        cout << "ratiosDivisor: " << ratiosDivisor << endl;
        for (int m = 0; m < numModalities; m++) {
            functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(nFeat, iota, topShift, idsShift,
                                                                             ratiosShift,
                                                                             topMask, topDivisor, idsMask,
                                                                             ratiosMask,
                                                                             ratiosDivisor);
        }
    } else {
        cout << "Different modality settings" << endl;
        for (int m = 0; m < numModalities; m++) {
            funcObjPy = PyList_GetItem(funcObjsPy, m);
            nFeat = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 0));
            cout << "nFeat: " << nFeat << endl;
            topShift = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 1));
            cout << "topShift: " << topShift << endl;
            idsShift = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 2));
            cout << "idsShift: " << idsShift << endl;
            ratiosShift = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 3));
            cout << "ratiosShift: " << ratiosShift << endl;
            topMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 4));
            cout << "topMask: " << topMask << endl;
            topDivisor = (double) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 5));
            cout << "topDivisor: " << topDivisor << endl;
            idsMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 6));
            cout << "idsMask: " << idsMask << endl;
            ratiosMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 7));
            cout << "ratiosMask: " << ratiosMask << endl;
            ratiosDivisor = (double) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 8));
            cout << "ratiosDivisor: " << ratiosDivisor << endl;
            functions[m] = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(nFeat, iota, topShift, idsShift,
                                                                             ratiosShift,
                                                                             topMask, topDivisor, idsMask,
                                                                             ratiosMask,
                                                                             ratiosDivisor);
        }
    }
    cout << "Loaded functions" << endl;
    //TODO: Could also make this an argument choice per modality to select eCP or H5 or something different.
    // Specific to ExqDataHandlerH5
    //for (int i = 0; i < PyList_Size(compCnfgFilesPy); i++) {
    //    PyObject* cnfgFilesPy = PyList_GetItem(compCnfgFilesPy, i);
    //    vector<string> cnfgFiles = vector<string>(PyList_Size(cnfgFilesPy));
    //    for (int j = 0; j < PyList_Size(cnfgFilesPy); j++) {
    //         cnfgFiles.emplace_back(_PyUnicode_AsString(PyList_GetItem(cnfgFilesPy,j)));
    //    }
    //    compCnfgFiles.push_back(cnfgFiles);
    //}
    //auto dataHandler = new ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>(compCnfgFiles, numModalities);
    vector<string> cnfgFiles = vector<string>(PyList_Size(compCnfgFilesPy));
    for (int i = 0; i < PyList_Size(compCnfgFilesPy); i++) {
        cnfgFiles[i] = _PyUnicode_AsString(PyList_GetItem(compCnfgFilesPy,i));
        cout << "Config[" << i << "]: " << cnfgFiles[i] << endl;
    }
    cout << "Initializing DataHandler" << endl;
    auto dataHandler =
            new ExqDataHandlerECP<uint64_t,uint64_t,uint64_t>(cnfgFiles, numModalities,
                                                              functions, modFeatureDimensions);
    cout << "DataHandler ready" << endl;
    vector<ExqClassifier*> classifiers = vector<ExqClassifier*>(numModalities);
    for (int m = 0; m < numModalities; m++) {
        classifiers[m] = new ExqClassifier(modFeatureDimensions[m]);
    }
    cout << "Classifier(s) initialized" << endl;
    auto worker = new ExqWorker<ExqDescriptor<uint64_t,uint64_t,uint64_t>>();
    cout << "Worker(s) initialized" << endl;

    PyObject* itemMetadataPy = PyTuple_GetItem(args, 10);
    PyObject* tmpPropsPy;
    auto itemProps = vector<ItemProperties>((int)PyList_Size(itemMetadataPy));
    cout << "Getting metadata" << endl;
    for (int i = 0; i < (int)itemProps.size(); i++) {
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_EXTRA)
        cout << "Item: " << i << endl;
#endif
        ItemProperties it = ItemProperties();
        it.collectionId = (uint16_t) PyLong_AsLong(PyList_GetItem(PyList_GetItem(itemMetadataPy,i),0));
        it.vid = (bool) PyLong_AsLong(PyList_GetItem(PyList_GetItem(itemMetadataPy,i),1));
        it.vidId = (int) PyLong_AsLong(PyList_GetItem(PyList_GetItem(itemMetadataPy,i),2));
        PyObject* stdPropsPy = PyList_GetItem(PyList_GetItem(itemMetadataPy,i),3);
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_EXTRA)
        cout << "Getting standard properties" << endl;
#endif
        it.stdProps.props = vector<set<uint16_t>>((int)PyList_Size(stdPropsPy));
        for (int j = 0; j < PyList_Size(stdPropsPy); j++) {
            tmpPropsPy = PyList_GetItem(stdPropsPy,j);
            auto props = set<uint16_t>();
            for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
               props.insert((uint16_t)PyLong_AsLong(PyList_GetItem(tmpPropsPy,k)));
            }
            it.stdProps.props[j] = props;
        }
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_EXTRA)
        cout << "Getting collection specific properties" << endl;
#endif
        PyObject* collPropsPy = PyList_GetItem(PyList_GetItem(itemMetadataPy,i),4);
        it.collProps.props = vector<set<uint16_t>>((int)PyList_Size(collPropsPy));
        for (int j = 0; j < PyList_Size(collPropsPy); j++) {
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_EXTRA)
            cout << "Collection prop: " << j << endl;
#endif
            tmpPropsPy = PyList_GetItem(collPropsPy, j);
            auto props = set<uint16_t>();
            for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
                props.insert((uint16_t)PyLong_AsLong(PyList_GetItem(tmpPropsPy,k)));
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_EXTRA)
                cout << "Prop: "  << j << "." << k << ": " << arr.getItem(k) << endl;
#endif
            }
            it.collProps.props[j] = props;
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_EXTRA)
            cout << "Prop " << j  << " set" << endl;
#endif
        }
        itemProps[i] = it;
    }
    cout << "Item metadata filled" << endl;

    PyObject* videoMetadataPy = PyTuple_GetItem(args, 11);
    auto collVidProps = vector<vector<Props>>((int)PyList_Size(videoMetadataPy));
    for (int c = 0; c < PyList_Size(videoMetadataPy); c++) {
        auto cVids = vector<Props>((int)PyList_Size(PyList_GetItem(videoMetadataPy,c)));
        for (int v = 0; v < (int)cVids.size(); v++) {
            PyObject* vidPropsPy = PyList_GetItem(videoMetadataPy,c);
            Props vp = Props();
            vp.props = vector<set<uint16_t>>((int)PyList_Size(vidPropsPy));
            for (int p = 0; p < (int)vp.props.size(); p++) {
                tmpPropsPy = PyList_GetItem(PyList_GetItem(vidPropsPy, v), p);
                auto props = set<uint16_t>();
                for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
                    props.insert((uint16_t) PyLong_AsLong(PyList_GetItem(tmpPropsPy, k)));
                }
                vp.props[p] = props;
            }
            cVids[v] = vp;
        }
        collVidProps[c] = cVids;
    }
    cout << "Video metadata filled" << endl;

    cout << "Initializing Controller" << endl;
    _pyExqV1._controller = new ExqController<ExqDescriptor<uint64_t,uint64_t,uint64_t>>(
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

    cout << "Controller initialized. Exquisitor is ready!" << endl;
    Py_IncRef(Py_None);
    return Py_None;
}

PyObject* exq::train_py([[maybe_unused]] PyObject* self, PyObject* args) {
    vector<uint32_t> trainIds = vector<uint32_t>();
    vector<float> trainLabels = vector<float>();
    Filters filters = Filters();

    PyObject* trainIdsPy = PyTuple_GetItem(args, 0);
    PyObject* trainLabelsPy = PyTuple_GetItem(args, 1);
    for (int i = 0; i < PyList_Size(trainIdsPy); i++) {
        trainIds.push_back((int)PyLong_AsLong(PyList_GetItem(trainIdsPy,i)));
        trainLabels.push_back((float)PyFloat_AsDouble(PyList_GetItem(trainLabelsPy,i)));
    }

    // Input Check
    if (trainIds.empty()) {
        cout << "No train items provided!" << endl;
        Py_IncRef(Py_None);
        return Py_None;
    }
    if (trainLabels.empty()) {
        cout << "No train labels provided!" << endl;
        Py_IncRef(Py_None);
        return Py_None;
    }
    if (trainLabels.size() != trainIds.size()) {
        cout << "Train items and labels mismatch!" << endl;
        Py_IncRef(Py_None);
        return Py_None;
    }

    //Change filters?
    bool changeFilters = (bool) PyBool_FromLong(PyLong_AsLong(PyTuple_GetItem(args,3)));
    // Set filters
    if (changeFilters) {
        PyObject* filtersPy = PyTuple_GetItem(args,4);
        filters.collection = set<uint16_t>();
        filters.stdFilters = vector<pair<int,set<uint16_t>>>();
        filters.collFilters = vector<pair<int,vector<pair<int,set<uint16_t>>>>>();
        filters.vidFilters = vector<pair<int,vector<pair<int,set<uint16_t>>>>>();
        for (int i = 0; i < (int) PyList_Size(PyList_GetItem(filtersPy,0)); i++) {

        }
    }

    auto times = _pyExqV1._controller->train(trainIds, trainLabels, filters, changeFilters);

    PyObject* timeList = PyList_New((int)times.size());
    for (int i = 0; i < (int)times.size(); i++) {
        PyList_SET_ITEM(timeList, i, PyFloat_FromDouble(times[i]));
    }

    return timeList;
}

PyObject* exq::suggest_py([[maybe_unused]] PyObject* self, PyObject* args) {
    PyObject* suggsPy;
    PyObject* totalPy;
    PyObject* workerTimesPy;
    PyObject* finalPy;
    PyObject* totalTimePy;
    PyObject* overheadTimePy;

    cout << "Getting suggestions" << endl;
    vector<uint32_t> seen = vector<uint32_t>();
    int r = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
    int segments = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
    TopResults top = TopResults(segments);
    PyObject* alreadySeenPy = PyTuple_GetItem(args, 2);

    for (int i = 0; i < PyList_Size(alreadySeenPy); i++) {
        seen.push_back((uint32_t)PyLong_AsLong(PyList_GetItem(alreadySeenPy,i)));
    }

    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    cout << "Calling suggest" << endl;
    top = _pyExqV1._controller->suggest(r, seen);
    cout << "Suggestions retrieved" << endl;
    time_point<high_resolution_clock> finish = high_resolution_clock::now();

    suggsPy = PyList_New((int)top.suggs.size());
    totalPy = PyList_New(segments);
    workerTimesPy = PyList_New(segments);
    finalPy = PyTuple_New(5);
    totalTimePy = PyFloat_FromDouble(duration<double, milli>(finish-begin).count());
    overheadTimePy = PyFloat_FromDouble(top.overheadTime);

    for (int i = 0; i < (int)top.suggs.size(); i++) {
        PyList_SetItem(suggsPy, i, PyLong_FromUnsignedLong((unsigned long)(top.suggs[i])));
    }
    for (int i = 0; i < segments; i++) {
        PyList_SetItem(totalPy, i, PyLong_FromUnsignedLong(top.totalItemsConsideredPerSegment[i]));
        PyList_SetItem(workerTimesPy, i, PyFloat_FromDouble(top.totalTimePerSegment[i]));
    }

    PyTuple_SetItem(finalPy, 0, suggsPy);
    PyTuple_SetItem(finalPy, 1, totalPy);
    PyTuple_SetItem(finalPy, 2, workerTimesPy);
    PyTuple_SetItem(finalPy, 3, totalTimePy);
    PyTuple_SetItem(finalPy, 4, overheadTimePy);

    return finalPy;
}

PyObject* exq::reset_model_py([[maybe_unused]] PyObject* self, [[maybe_unused]] PyObject* args) {
    _pyExqV1._controller->reset_model();

    Py_IncRef(Py_None);
    return Py_None;
}

PyObject* exq::safe_exit_py([[maybe_unused]] PyObject* self, [[maybe_unused]] PyObject* args) {
    //TODO: Call terminate function
    Py_IncRef(Py_None);
    return Py_None;
}

PyObject* exq::get_descriptors_info_py([[maybe_unused]] PyObject *self, PyObject *args) {
    PyObject* descIdsPy;
    descIdsPy = PyTuple_GetItem(args,0);
    vector<int> ids = vector<int>(PyList_Size(descIdsPy));
    for (int i = 0; i < PyList_Size(descIdsPy); i++) {
        ids[i] = (int) PyLong_AsLong(PyList_GetItem(descIdsPy,i));
    }
    int modality = (int) PyLong_AsLong(PyTuple_GetItem(args,1));
    auto res = _pyExqV1._controller->get_descriptors(ids, modality);

    PyObject* descListPy;
    descListPy = PyList_New((int)ids.size());
    for (int i = 0; i < (int)ids.size(); i++) {
        PyObject* descInfo = PyList_New(res[i].getSize());
        for (int j = 0; j < res[i].getSize(); j++) {
            PyObject* pair = PyTuple_New(2);
            PyTuple_SetItem(pair, 0, PyLong_FromUnsignedLong((unsigned long) res[i].getItem(j).first));
            PyTuple_SetItem(pair, 1, PyFloat_FromDouble((double) res[i].getItem(j).second));
            PyList_SetItem(descInfo, j, pair);
        }
        PyList_SetItem(descListPy, i, descInfo);
    }

    return descListPy;
}

[[maybe_unused]] PyMODINIT_FUNC exq::PyInit_exq(void) {
    Py_Initialize();
    return PyModule_Create(&exquisitor_definition);
}