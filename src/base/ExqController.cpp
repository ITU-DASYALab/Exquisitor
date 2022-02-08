//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqController.h"
//#include "../ExqFunctionsR64.h"

#include <future>
#include <utility>

using namespace exq;
using std::async;
using std::future_status;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::cout;
using std::endl;


template <typename T>
ExqController<T>::ExqController(
        int iota,
        int noms,
        int workers,
        int segments,
        int numberModalities,
        vector<int> modFeatureDimensions,
        int bClusters,
        vector<ExqFunctions<T>*> functions,
        ExqDataHandler<T>* handler,
        vector<ExqClassifier*> classifiers,
        ExqWorker<T>* worker,
        const ExqArray<ItemProperties>& itemProps,
        const ExqArray<ExqArray<Props>>& vidProps
    ) {
    cout << "(CTRL) Setting parameters" << endl;
    // Set standard fields
    _iota = iota;
    _noms = noms;
    _numWorkers = workers;
    _segments = segments;
    _modalities = numberModalities;
    _bClusters = bClusters;
    _featureDimensions = std::move(modFeatureDimensions);

    // Set exq class object fields
    _functions = functions;
    _handler = handler;
    _classifiers = std::move(classifiers);
    _worker = worker;
    _itemProperties = itemProps;
    _vidProperties = vidProps;

    cout << "(CTRL) Loading data..." << endl;
    // Load data
    _handler->loadData(_modalities);
    cout << "(CTRL) Data loaded" << endl;

    _threads.resize(_numWorkers);
}


template <typename T>
vector<double> ExqController<T>::train(vector<uint> trainIds, vector<short> trainLabels) {
    vector<double> times = vector<double>();
    vector<vector<double>> weights = vector<vector<double>>(_modalities, vector<double>());
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    time_point<high_resolution_clock> finish = high_resolution_clock::now();

    for (int m = 0; m < _modalities; m++) {
        vector<vector<double>> trainingItems = vector<vector<double>>();
        for (int i = 0; i < trainIds.size(); i++) {
            T desc = _handler->getDescriptor(i);
            ExqArray<pair<int, float>> descVals = _functions[m]->getDescriptorInformation(desc);
            vector<double> featVals = vector<double>(_classifiers[m]->getTotalFeats(), 0.0);
            for (int j = 0; j < descVals.getSize(); j++) {
                pair<int, float> item = descVals.getItem(i);
                featVals[item.first] = item.second;
            }
            trainingItems.push_back(featVals);
        }

        finish = high_resolution_clock::now();
        times.push_back(duration<double, milli>(finish - begin).count());
        begin = high_resolution_clock::now();
        _classifiers[m]->train(trainingItems, trainLabels);
        weights[m] = _classifiers[m]->getWeights();
        finish = high_resolution_clock::now();
        times.push_back(duration<double, milli>(finish - begin).count());
    }
    begin = high_resolution_clock::now();
    vector<int> bPerMod;
    vector<double> bias;
    for (int m = 0; m < _modalities; m++) {
        bPerMod.push_back(_bClusters);
        bias.push_back(_classifiers[m]->getBias());
    }

    _handler->selectClusters(bPerMod, weights, bias);
    finish = high_resolution_clock::now();
    times.push_back(duration<double, milli>(finish - begin).count());
    return times;
}


template <typename T>
TopResults ExqController<T>::suggest(int k, vector<uint> seenItems) {
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    TopResults results = TopResults();
    int completedSegments = 0;
    int runningSegments = 0;
    int workerSegments[_numWorkers];
    for (int i = 0; i < _numWorkers; i++) {
        workerSegments[i] = -1;
    }
    unordered_set<uint> seenSet;
    for (unsigned int & seenItem : seenItems) {
        seenSet.insert(seenItem);
    }
    vector<ExqItem> items2Return;

    while (completedSegments < _segments) {
        for (int w = 0; w < _numWorkers; w++) {
            if (workerSegments[w] == -1 && runningSegments < _segments) {
                _threads[w] = async(std::launch::async, [&] {
                    return _worker->suggest(k, items2Return,
                                            _classifiers,
                                            runningSegments, _segments,
                                            _noms, _modalities, _handler, _functions, seenSet,
                                            results.totalTimePerSegment[runningSegments],
                                            results.totalItemsConsideredPerSegment[runningSegments], w);
                });
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
    completedSegments = 0;

    //TODO: Duplicate check
    _functions[0]->sortItems(items2Return, _modalities);

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


