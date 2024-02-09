//
// Created by Omar Shahbaz Khan on 11/05/2020.
//

#include "ExqPyInterface.h"
#include "r64/ExqDataHandlerECP.h"
#include "r64/ExqFunctionsR64.h"
#include "ExqPyHelperFunctions.h"

#include <cstdlib>

using namespace exq;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::cout;
using std::endl;
using std::unordered_set;

PyObject* exq::initialize_py([[maybe_unused]] PyObject* self, PyObject* args) {
    cout << "Initializing Exquisitor... " << endl;

    _pyExqV1 = PyExquisitorV1();

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
    auto functions = vector<IExqFunctions<uint64_t>*>(numModalities);
    int funcType = (int) PyLong_AsLong(PyTuple_GetItem(args, 8));
    cout << "funcType: " << funcType << endl;
    //funcType: 0 = same; 1 = different;
    PyObject* funcObjsPy = PyTuple_GetItem(args, 9);
    //funcObj: 0 = nFeat, 1 = topShift, 2 = idsShift, 3 = ratiosShift, 4 = topMask,
    //         5 = topDivisor, 6 = idsMask, 7 = ratiosMask, 8 = ratiosDivisor
    int nFeat, topShift, idsShift, ratiosShift;
    float topDivisor, ratiosDivisor;
    // TODO: This should be part of a helper class later on, but for now belongs in Controller
    vector<float> modalityWeights;
    cout << "Creating functions object" << endl;
    PyObject* funcObjPy;
    if (funcType == 0) {
        uint64_t topMask, idsMask, ratiosMask;
        cout << "Different modality settings" << endl;
        modalityWeights = vector<float>(numModalities);
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
            topDivisor = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 5));
            cout << "topDivisor: " << topDivisor << endl;
            idsMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 6));
            cout << "idsMask: " << idsMask << endl;
            ratiosMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 7));
            cout << "ratiosMask: " << ratiosMask << endl;
            ratiosDivisor = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 8));
            cout << "ratiosDivisor: " << ratiosDivisor << endl;
            modalityWeights[m] = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 9));
            cout << "Modality " << m << " weight: " << modalityWeights[m] << endl;
            functions[m] = new ExqFunctionsR64(nFeat, iota, topShift, idsShift,
                                               ratiosShift,
                                               topMask, topDivisor, idsMask,
                                               ratiosMask, ratiosDivisor);
        }
   } else if (funcType == 1) {
        uint64_t topMask, idsMask, ratiosMask;
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
        topDivisor = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 5));
        cout << "topDivisor: " << topDivisor << endl;
        idsMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 6));
        cout << "idsMask: " << idsMask << endl;
        ratiosMask = (uint64_t) PyLong_AsLong(PyList_GetItem(funcObjPy, 7));
        cout << "ratiosMask: " << ratiosMask << endl;
        ratiosDivisor = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 8));
        cout << "ratiosDivisor: " << ratiosDivisor << endl;
        auto modWeight = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 9));
        modalityWeights = vector<float>(numModalities, modWeight);
        for (int m = 0; m < numModalities; m++) {
            functions[m] = new ExqFunctionsR64(nFeat, iota, topShift, idsShift,
                                               ratiosShift,
                                               topMask, topDivisor, idsMask,
                                               ratiosMask, ratiosDivisor);
            cout << "Modality " << m << " weight: " << modalityWeights[m] << endl;
        }
    } else if (funcType == 2) {
        int topMask, idsMask, ratiosMask;
        cout << "Different modality settings" << endl;
        modalityWeights = vector<float>(numModalities);
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
            topMask = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 4));
            cout << "topMask: " << topMask << endl;
            topDivisor = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 5));
            cout << "topDivisor: " << topDivisor << endl;
            idsMask = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 6));
            cout << "idsMask: " << idsMask << endl;
            ratiosMask = (int) PyLong_AsLong(PyList_GetItem(funcObjPy, 7));
            cout << "ratiosMask: " << ratiosMask << endl;
            ratiosDivisor = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 8));
            cout << "ratiosDivisor: " << ratiosDivisor << endl;
            modalityWeights[m] = (float) PyFloat_AsDouble(PyList_GetItem(funcObjPy, 9));
            cout << "Modality " << m << " weight: " << modalityWeights[m] << endl;
            functions[m] = new ExqFunctionsR64(nFeat, iota, topShift, idsShift,
                                               ratiosShift,
                                               topMask, topDivisor, idsMask,
                                               ratiosMask, ratiosDivisor);
        }
    }
    assert(!modalityWeights.empty());

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
    vector<ExqClassifier*> classifiers = vector<ExqClassifier*>(numModalities);
    for (int m = 0; m < numModalities; m++) {
        classifiers[m] = new ExqClassifier(modFeatureDimensions[m]);
    }
    cout << "Classifier(s) initialized" << endl;
    auto worker = new ExqWorker<uint64_t>();
    cout << "Worker(s) initialized" << endl;

    PyObject* itemMetadataPy = PyTuple_GetItem(args, 10);
    PyObject* tmpPropsPy;
    auto itemProps = vector<ItemProperties>((int)PyList_Size(itemMetadataPy));
    cout << "Getting metadata" << endl;
    for (int i = 0; i < (int)itemProps.size(); i++) {
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_META)
        cout << "Item: " << i << endl;
#endif
        ItemProperties it = ItemProperties();
        it.collectionId = (uint16_t) PyLong_AsLong(PyList_GetItem(PyList_GetItem(itemMetadataPy,i),0));
        it.vid = (bool) PyLong_AsLong(PyList_GetItem(PyList_GetItem(itemMetadataPy,i),1));
        it.vidId = (int) PyLong_AsLong(PyList_GetItem(PyList_GetItem(itemMetadataPy,i),2));
        PyObject* stdPropsPy = PyList_GetItem(PyList_GetItem(itemMetadataPy,i),3);
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_META)
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
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_META)
        cout << "Getting collection specific properties" << endl;
#endif
        PyObject* collPropsPy = PyList_GetItem(PyList_GetItem(itemMetadataPy,i),4);
        it.collProps.props = vector<set<uint16_t>>((int)PyList_Size(collPropsPy));
        for (int j = 0; j < PyList_Size(collPropsPy); j++) {
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_META)
            cout << "Collection prop: " << j << endl;
#endif
            tmpPropsPy = PyList_GetItem(collPropsPy, j);
            auto props = set<uint16_t>();
            for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
                props.insert((uint16_t)PyLong_AsLong(PyList_GetItem(tmpPropsPy,k)));
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_META)
                //cout << "Prop: "  << j << "." << k << ": " << props.getItem(k) << endl;
#endif
            }
            it.collProps.props[j] = props;
#if defined (DEBUG_EXTRA) || defined(DEBUG_INIT_META)
            cout << "Prop " << j  << " set" << endl;
#endif
        }

        PyObject* countPropsPy = PyList_GetItem(PyList_GetItem(itemMetadataPy,i),5);
        it.countProps.props = vector<map<uint16_t,uint16_t>>((int) PyList_Size(countPropsPy));
        for (int j = 0; j < PyList_Size(countPropsPy); j++) {
            tmpPropsPy = PyList_GetItem(countPropsPy,j);
            auto props = map<uint16_t,uint16_t>();
            for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
                PyObject* mapArrPy = PyList_GetItem(tmpPropsPy,k);
                for (int l = 0; l < PyList_Size(mapArrPy); l++) {
                    auto objectId = (uint16_t) PyLong_AsLong(PyList_GetItem(PyList_GetItem(mapArrPy,l),0));
                    auto count = (uint16_t) PyLong_AsLong(PyList_GetItem(PyList_GetItem(mapArrPy,l),1));
                    props[objectId] = count;
                    if (i < 4) {
                        cout << "Item: " << i << " | ObjectId: " << objectId << " | Count: " << count << endl;
                    }
                }
                //PyObject* repr = PyObject_Repr();
                //PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
                //const char *bytes = PyBytes_AS_STRING(str);
                //printf("REPR: %s\n", bytes);
                //Py_XDECREF(repr);
                //Py_XDECREF(str);
            }
            it.countProps.props[j] = props;
        }
        itemProps[i] = it;
    }
    cout << "Item metadata filled" << endl;

    PyObject* videoMetadataPy = PyTuple_GetItem(args, 11);
    auto collVidProps = vector<vector<Props>>((int)PyList_Size(videoMetadataPy));
    // Go through collections
    for (int c = 0; c < PyList_Size(videoMetadataPy); c++) {
        collVidProps[c] = vector<Props>((int)PyList_Size(PyList_GetItem(videoMetadataPy,c)));
        PyObject* videosPy = PyList_GetItem(videoMetadataPy,c);
        // cout << "Videos: " << collVidProps[c].size() << endl;
        // Go through videos of collection c
        for (int v = 0; v < (int)collVidProps[c].size(); v++) {
            // Video properties
            PyObject* vidPropsPy = PyList_GetItem(videosPy,v);
            collVidProps[c][v] = Props();
            collVidProps[c][v].props = vector<set<uint16_t>>((int)PyList_Size(vidPropsPy));
            // Go through properties and set values
            for (int p = 0; p < (int)PyList_Size(vidPropsPy); p++) {
                tmpPropsPy = PyList_GetItem(vidPropsPy, p);
                auto props = set<uint16_t>();
                for (int k = 0; k < PyList_Size(tmpPropsPy); k++) {
                    props.insert((uint16_t) PyLong_AsLong(PyList_GetItem(tmpPropsPy, k)));
                }
                collVidProps[c][v].props[p] = props;
            }
        }
    }
    cout << "Video metadata filled" << endl;

    ExpansionType expType;
    int exp = (int)PyLong_AsLong(PyTuple_GetItem(args, 12));
    switch (exp) {
        case 1:
            expType = ExpansionType::GLOBAL_REMAINING_CNT;
            break;
        case 2:
            expType = ExpansionType::FILTER_REMAINING_CNT;
            break;
        case 3:
            expType = ExpansionType::ESTIMATED_REMAINING_CNT;
            break;
        case 4:
            expType = ExpansionType::ALL_REMAINING_CNT;
            break;
        default:
            expType = ExpansionType::ORIGINAL_CNT;
    }
    int statLevel = (int)PyLong_AsLong(PyTuple_GetItem(args, 13));
    cout << "statLevel: " << statLevel << endl;

    bool ffs = (bool)PyLong_AsLong(PyTuple_GetItem(args, 14));
    cout << "ffs: " << ffs << endl;
    int guaranteedSlots = (int)PyLong_AsLong(PyTuple_GetItem(args, 15));
    cout << "guaranteedSlots: " << guaranteedSlots << endl;

    vector<string> cnfgFiles = vector<string>(PyList_Size(compCnfgFilesPy));
    for (int i = 0; i < PyList_Size(compCnfgFilesPy); i++) {
        cnfgFiles[i] = _PyUnicode_AsString(PyList_GetItem(compCnfgFilesPy,i));
        cout << "Config[" << i << "]: " << cnfgFiles[i] << endl;
    }
    cout << "Initializing DataHandler" << endl;
    auto dataHandler =
            new ExqDataHandlerECP(cnfgFiles, numModalities,
                                  functions, modFeatureDimensions,
                                  itemProps, collVidProps,
                                  expType, statLevel);
    cout << "DataHandler ready" << endl;

    cout << "Initializing Controller" << endl;
    _pyExqV1._controller = new ExqController<uint64_t>(
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
            // itemProps,
            // collVidProps,
            modalityWeights,
            ffs,
            guaranteedSlots
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
        trainIds.push_back((int) PyLong_AsLong(PyList_GetItem(trainIdsPy,i)));
        trainLabels.push_back((float) PyFloat_AsDouble(PyList_GetItem(trainLabelsPy,i)));
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
    bool changeFilters = (bool) PyLong_AsLong(PyTuple_GetItem(args,2));
    // Set filters
    if (changeFilters) {
        //cout << "Getting filters" << endl;
        PyObject* filtersPy = PyTuple_GetItem(args,3);
        extractFiltersFromPythonObject(filtersPy, filters);
        //cout << "Set filters" << endl;
    }
    cout << "Modality weight option" << endl;
    //update modality fusion weights?
    bool updateModWeights = (bool) PyLong_AsLong(PyTuple_GetItem(args,4));
    bool modalityWeightsOk = true;
    if (updateModWeights) {
        modalityWeightsOk = _pyExqV1._controller->update_modality_weights(trainIds, trainLabels);
    }

    int returnSize = (_pyExqV1._controller->getNumModalites() * 2) + 2;
    PyObject* timeList = PyList_New(returnSize);
    if (modalityWeightsOk) {
        cout << "(EXQ) Training" << endl;
        auto times = _pyExqV1._controller->train(trainIds, trainLabels, changeFilters, filters);
#if defined(DEBUG) || defined(DEBUG_TRAIN)
        cout << "Creating return object" << endl;
#endif
        for (int i = 0; i < (int)times.size(); i++) {
            PyList_SetItem(timeList, i, PyFloat_FromDouble(times[i]));
#if defined(DEBUG) || defined(DEBUG_TRAIN)
            cout << times[i] << endl;
#endif
        }
        PyList_SetItem(timeList, times.size(), PyBool_FromLong(modalityWeightsOk));
    } else {
        for (int i = 0; i < returnSize-1; i++) {
            PyList_SetItem(timeList, i, PyFloat_FromDouble(0.0));
        }
        PyList_SetItem(timeList, returnSize-1, PyBool_FromLong(modalityWeightsOk));
    }
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "Returning" << endl;
#endif
    return timeList;
}

PyObject* exq::suggest_py([[maybe_unused]] PyObject* self, PyObject* args) {
    PyObject* suggsPy;
    PyObject* totalPy;
    PyObject* workerTimesPy;
    PyObject* finalPy;
    PyObject* totalTimePy;
    PyObject* overheadTimePy;

    Filters filters = Filters();

    cout << "Getting suggestions" << endl;
    vector<uint32_t> seen = vector<uint32_t>();
    int r = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
    int segments = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
    TopResults top = TopResults(segments);
    PyObject* alreadySeenPy = PyTuple_GetItem(args, 2);

    bool changeFilters = (bool) PyLong_AsLong(PyTuple_GetItem(args,3));
    if (changeFilters) {
        PyObject* filtersPy = PyTuple_GetItem(args,4);
        extractFiltersFromPythonObject(filtersPy, filters);
    }

    for (int i = 0; i < PyList_Size(alreadySeenPy); i++) {
        seen.push_back((uint32_t)PyLong_AsLong(PyList_GetItem(alreadySeenPy,i)));
    }

    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    cout << "Calling suggest" << endl;
    top = _pyExqV1._controller->suggest(r, seen, changeFilters, filters);
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
    //cout << "Resetting model" << endl;
    bool resetSession = (bool) PyLong_AsLong(PyTuple_GetItem(args, 0));
    bool resetModalityWeights = (bool) PyLong_AsLong(PyTuple_GetItem(args, 1));
    if (resetModalityWeights) {
        _pyExqV1._controller->reset_modality_weights();
    }
    _pyExqV1._controller->reset_model(resetSession);

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
    descIdsPy = PyTuple_GetItem(args, 0);
    auto ids = vector<int>(PyList_Size(descIdsPy));
    cout << "Get info for " << (int) PyList_Size(descIdsPy) << endl;
    for (int i = 0; i < (int) PyList_Size(descIdsPy); i++) {
        ids[i] = (int) PyLong_AsLong(PyList_GetItem(descIdsPy, i));
        cout << "Image ids: " << ids[i] << endl;
    }
    int modality = (int) PyLong_AsLong(PyTuple_GetItem(args, 1));
    auto res = _pyExqV1._controller->get_descriptors(ids, modality);
    cout << "Got info" << endl;

    PyObject* descListPy;
    descListPy = PyList_New((int)ids.size());
    for (int i = 0; i < (int)ids.size(); i++) {
        int resSize = res[i].getSize();
        PyObject* descInfo = PyList_New(resSize);
        for (int j = 0; j < resSize; j++) {
            PyObject* pair = PyTuple_New(2);
            PyTuple_SetItem(pair, 0, PyLong_FromUnsignedLong((unsigned long) res[i].getItem(j).first));
            PyTuple_SetItem(pair, 1, PyFloat_FromDouble(res[i].getItem(j).second));
            PyList_SetItem(descInfo, j, pair);
        }
        PyList_SetItem(descListPy, i, descInfo);
    }

    return descListPy;
}

PyMODINIT_FUNC exq::PyInit_exq(void) {
    // cout << "Initializing exq module" << endl;
    // cout << std::getenv("PYTHONLIBS") << endl;
    // Py_Initialize();
    if (!Py_IsInitialized()) {
        cout << "Initializing Python Interpreter" << endl;
        Py_Initialize();
    }
    return PyModule_Create(&exq_definition);
}