//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqController.h"
#include "ExqFunctionsR64.h"
#include <future>

using namespace exq;
using std::async;
using std::future_status;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;


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
        ExqWorker<T>* worker
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
vector<double> ExqController<T>::train(vector<uint32_t> trainIds, vector<short> trainLabels) {
    vector<double> times = vector<double>();
    vector<vector<double>> trainingItems = vector<vector<double>>();
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    time_point<high_resolution_clock> finish = high_resolution_clock::now();

    for (int i = 0; i < trainIds.size(); i++) {
        T desc = _handler->getDescriptor(i);
        ExqArray<pair<int,float>> descVals = _functions->getDescriptorInformation(desc);
        vector<double> featVals = vector<double>(N_TOTAL_FEAT, 0.0);
        for (int j = 0; j < descVals.getSize(); j++) {
            pair<int,float> item = descVals.getItem(i);
            featVals[item.first] = item.second;
        }
        trainingItems.push_back(featVals);
    }
    finish = high_resolution_clock::now();
    times.push_back(duration<double, milli>(finish - begin).count());
    begin = high_resolution_clock::now();
    _classifier->train(trainingItems, trainLabels);
    vector<double> weights = _classifier->getWeights();
    finish = high_resolution_clock::now();
    times.push_back(duration<double, milli>(finish - begin).count());
    begin = high_resolution_clock::now();
    _handler->selectClusters(_bClusters, weights, _classifier->getBias(), *_functions);
    finish = high_resolution_clock::now();
    times.push_back(duration<double, milli>(finish - begin).count());
    return times;
}


template <typename T>
TopResults ExqController<T>::suggest(int k, vector<uint32_t> seenItems) {
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    TopResults results = TopResults();
    int completedSegments = 0;
    int runningSegments = 0;
    int workerSegments[_numWorkers];
    for (int i = 0; i < _numWorkers; i++) {
        workerSegments[i] = -1;
    }
    unordered_set<uint32_t> seenSet;
    for (int i = 0; i < seenItems.size(); i++) {
        seenSet.insert(seenItems[i]);
    }
    vector<ExqItem> items2Return;

    while (completedSegments < _segments) {
        for (int w = 0; w < _numWorkers; w++) {
            if (workerSegments[w] == -1 && runningSegments < _segments) {
                _threads[w] = async(std::launch::async, [&] { return _worker->suggest(k, items2Return,
                                _classifier->getWeights(),_classifier->getBias(), runningSegments, _segments,
                                _noms, _modalities, _handler, _functions, seenSet,
                                results.totalTimePerSegment[runningSegments],
                                results.totalItemsConsideredPerSegment[runningSegments]); });
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
    //TODO: Duplicate check
    _functions->sortItems(items2Return, _modalities);

    for (int i = 0; i < k; i++) {
        results.suggs.push_back(items2Return[i].itemId);
    }

    time_point<high_resolution_clock> finish = high_resolution_clock::now();
    results.overheadTime = duration<double, milli>(finish - begin).count();
    return results;
}

template <typename T>
void ExqController<T>::reset_model() {

}
template class exq::ExqController<exq::ExqDescriptor<uint64_t,uint64_t,uint64_t>>;


