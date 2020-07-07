//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqController.h"
#include "ExqFunctionsR64.h"
#include <thread>
#include <future>

using namespace exq;
using std::async;
using std::future_status;

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
    _iota = iota;
    _noms = noms;
    _numWorkers = workers;
    _segments = segments;
    _modalities = numberModalities;
    _bClusters = bClusters;
    _featureDimensions = modFeatureDimensions;

    //Set exq class object fields
    _functions = functions;
    _handler = handler;
    _classifier = classifier;
    _worker = worker;

    //Load data
    _handler->loadData(_modalities, 1);

    _threads.resize(_numWorkers);
}

template <typename T>
//TODO: Come up with more descriptive signature argument names
vector<double> ExqController<T>::train(uint32_t* newXI, double* newY, int nNewTrain) {
    vector<double> times = vector<double>();
    vector<vector<double>> data = vector<vector<double>>();
    vector<short> labels = vector<short>();
    void (*dist)(vector<double>&, double, T&) = _functions->distance;

    //TODO: Get descriptor informations for positive and negatives and store them in data

    _classifier->train(data, labels);
    _handler->selectClusters(_bClusters, _classifier->getWeights(), _classifier->getBias(), dist);
    return times;
}

template <typename T>
TopResults ExqController<T>::suggest(int k, vector<uint32_t> seenItems) {
    TopResults results = TopResults();
    int completedSegments = 0;
    int runningSegments = 0;
    int workerSegments[_numWorkers];
    for (int i = 0; i < _numWorkers; i++) {
        workerSegments[i] = -1;
    }
    vector<ExqItem> items2Return;
    while (completedSegments < _segments) {
        for (int w = 0; w < _numWorkers; w++) {
            if (workerSegments[w] == -1 && runningSegments < _segments) {
                _threads[w] = async(_worker->suggest(k, items2Return, _classifier->getWeights(),
                                    _classifier->getBias(), runningSegments, _segments, _noms, _modalities,
                                    _handler, _functions));
                workerSegments[w] = runningSegments;
                runningSegments++;
            } else if (workerSegments[w] != -1 &&
                       _threads[w].wait_for(std::chrono::seconds(0)) == future_status::ready) {
                _threads[w].get();
                workerSegments[w] = -1;
                completedSegments++;
            }
        }
    }

    _functions->sortItems(items2Return, _modalities);

    for (int i = 0; i < k; i++) {
        results.suggs.push_back(items2Return[i].itemId);
    }

    return results;
}


