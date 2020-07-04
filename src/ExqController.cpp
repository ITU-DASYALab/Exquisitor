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
    void (*dist)(vector<double>&, double, T&) = this->_functions.distance;

    //TODO: Get descriptor informations for positive and negatives and store them in data

    this->_classifier->train(data, labels);
    this->_handler->selectClusters(this->_bClusters, this->_classifier->getWeights(), this->_classifier->getBias(), dist);
    return times;
}

template <typename T>
TopResults ExqController<T>::suggest(int k, vector<uint32_t> seenItems) {
    TopResults results = TopResults();

    vector<ExqItem> items2Return;
    //TODO: Fix Loop logic
    for (int s = 0; s < this->_segments; s++) {
        for (int w = 0; w < this->_numWorkers; w++) {
            //TODO: Utilize Thread to collect segment suggestions from ExqWorker
            _worker->suggest(k, items2Return, _classifier->getWeights(), _classifier->getBias(), s, this->_segments,
                             this->_noms, this->_modalities, this->_handler, this->_functions);
        }
    }

    return results;
}


