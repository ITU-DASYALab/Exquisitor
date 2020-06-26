//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqController.h"
#include "ExqFunctionsR64.h"

using namespace exq;

template <typename T>
ExqController<T>::ExqController(
        int iota,
        int noms,
        int workers,
        int segments,
        int numberModalities,
        vector<int> modFeatureDimensions,
        //vector<vector<string>> compFiles,
        //vector<string> cnfgFiles,
        int bClusters,
        ExqFunctions<T>* functions,
        ExqDataHandler<T>* handler,
        ExqClassifier* classifier,
        const ExqWorker* worker
    ) {

    //Set standard fields
    this->_iota = iota;
    this->_noms = noms;
    this->_numWorkers = workers;
    this->_segments = segments;
    this->_modalities = numberModalities;
    this->_bClusters = bClusters;
    this->_featureDimensions = modFeatureDimensions;

    //Set exq class object fields
    this->_functions = functions;
    this->_handler = handler;
    this->_classifier = classifier;
    this->_worker = worker;

    //Load data
    this->_handler->loadData(this->_modalities, 1);
}

template <typename T>
//TODO: Come up with more descriptive signature argument names
vector<double> ExqController<T>::train(uint32_t* newXI, double* newY, int nNewTrain) {
    vector<double> times = vector<double>();
    vector<vector<double>> data = vector<vector<double>>();
    vector<short> labels = vector<short>();

    //TODO: Get descriptor informations for positive and negatives and store them in data

    this->_classifier->train(data, labels);
    return times;
}


