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
        const vector<ItemProperties>& itemProps,
        const vector<vector<Props>>& vidProps
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
    _activeFilters = ItemFilter();

    // Set exq class object fields
    _functions = functions;
    _handler = handler;
    _classifiers = std::move(classifiers);
    _worker = worker;
    _itemProperties = itemProps;
#if defined(DEBUG) || defined(DEBUG_INIT_META)
    if (_itemProperties.size() != 0) {
        cout << "Checking item metadata" << endl;
        for(int i = 0; i < 3; i++) {
            cout <<"Item: " << i << " | ";
            cout <<"Collection Id: " << _itemProperties[i].collectionId << " | ";
            cout <<"Video Id: " << _itemProperties[i].vidId << " | ";
            cout << "Standard Props: ";
            for (int j = 0; j < (int)_itemProperties[i].stdProps.props.size(); j++) {
                cout << i << "." << j << " = [";
                for (auto elem : _itemProperties[i].stdProps.props[j]) {
                    cout << elem << ", ";
                }
                cout << "] ";
            }
            cout << "| ";
            cout << "Collection Props: ";
            for (int j = 0; j < (int)_itemProperties[i].collProps.props.size(); j++) {
                cout << i << "." << j << " = [";
                for (auto elem : _itemProperties[i].collProps.props[j]) {
                    cout << elem << ", ";
                }
                cout << "] ";
            }
            cout << endl;
        }
    }
#endif
    _vidProperties = vidProps;

    cout << "(CTRL) Loading data..." << endl;
    // Load data
    _handler->loadData(_modalities);
    cout << "(CTRL) Data loaded" << endl;

    _threads.resize(_numWorkers);
}


template <typename T>
vector<double> ExqController<T>::train(const vector<uint32_t>& trainIds, const vector<float>& trainLabels,
                                       bool changeFilters, Filters filters) {
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(CTRL) In train" << endl;
#endif
    vector<double> times = vector<double>();
    vector<vector<double>> weights = vector<vector<double>>(_modalities, vector<double>());
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    time_point<high_resolution_clock> finish = high_resolution_clock::now();

    if (changeFilters)
        _activeFilters.setFilters(filters);

    for (int m = 0; m < _modalities; m++) {
        vector<vector<double>> trainingItems = vector<vector<double>>();
        for (int i = 0; i < (int)trainIds.size(); i++) {
            T desc = _handler->getDescriptor(trainIds[i]);
            ExqArray<pair<int, float>> descVals = _functions[m]->getDescriptorInformation(desc);
            vector<double> featVals = vector<double>(_classifiers[m]->getTotalFeats(), 0.0);
            for (int j = 0; j < descVals.getSize(); j++) {
                pair<int, float> item = descVals.getItem(j);
                featVals[item.first] = item.second;
            }
            trainingItems.push_back(featVals);
        }
        finish = high_resolution_clock::now();
        times.push_back(duration<double, milli>(finish - begin).count());
        begin = high_resolution_clock::now();
        weights[m] = _classifiers[m]->train(trainingItems, trainLabels);
        //weights[m] = _classifiers[m]->getWeights();
        finish = high_resolution_clock::now();
        times.push_back(duration<double, milli>(finish - begin).count());
    }
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(CTRL) Classifier trained" << endl;
#endif
    begin = high_resolution_clock::now();
    vector<int> bPerMod;
    vector<double> bias;
    for (int m = 0; m < _modalities; m++) {
        bPerMod.push_back(_bClusters);
        bias.push_back(_classifiers[m]->getBias());
    }
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(CTRL) Selecting clusters" << endl;
#endif
    _handler->selectClusters(bPerMod, weights, bias, _activeFilters);
    finish = high_resolution_clock::now();
    times.push_back(duration<double, milli>(finish - begin).count());
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(CTRL) Training done" << endl;
#endif
    return times;
}


template <typename T>
TopResults ExqController<T>::suggest(int k, const vector<uint32_t>& seenItems, bool useActiveFilters,
                                     const Filters& filters) {
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(CTRL) Setting suggest parameters" << endl;
#endif
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    auto results = TopResults(_segments);
    int completedSegments = 0;
    int runningSegments = 0;
    int workerSegments[_numWorkers];
    for (int w = 0; w < _numWorkers; w++) {
        workerSegments[w] = -1;
    }
    auto seenSet = unordered_set<uint32_t>();
    for (const uint32_t& seenItem : seenItems) {
        seenSet.insert(seenItem);
    }
    auto itemsFromSegments = vector<vector<ExqItem>>(_segments);
    int totalItemsReturned = 0;

    ItemFilter usedFilters;
    useActiveFilters ? usedFilters = _activeFilters : usedFilters = ItemFilter(filters);

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(CTRL) Starting workers" << endl;
#endif
    while (completedSegments < _segments) {
        for (int w = 0; w < _numWorkers; w++) {
            if (workerSegments[w] == -1 && runningSegments < _segments) {
                workerSegments[w] = runningSegments;
                itemsFromSegments[workerSegments[w]] = vector<ExqItem>();
                _threads[w] = async(std::launch::async, [&] {
                    return _worker->suggest(k, itemsFromSegments[workerSegments[w]],
                                            _classifiers,
                                            workerSegments[w], _segments,
                                            _noms, _modalities, _handler, _functions, seenSet,
                                            results.totalTimePerSegment[workerSegments[w]],
                                            results.totalItemsConsideredPerSegment[workerSegments[w]], w,
                                            usedFilters);
                });
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
                cout << "(CTRL) Running segment " << workerSegments[w] << endl;
#endif
                runningSegments++;
            } else if (workerSegments[w] != -1 &&
                       _threads[w].wait_for(std::chrono::seconds(0)) == future_status::ready) {
                _threads[w].get();
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
                cout << "(CTRL) Worker " << w << " done with segment " << workerSegments[w] << endl;
#endif
                totalItemsReturned += itemsFromSegments[workerSegments[w]].size();
                workerSegments[w] = -1;
                completedSegments++;
            }
        }
    }
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(CTRL) Workers done" << endl;
    cout << "(CTRL) Sanity check: " << itemsFromSegments[0][0].itemId << " " << itemsFromSegments[0][0].distance[0] << endl;
#endif
    auto items2Return = vector<ExqItem>();
    items2Return.reserve(totalItemsReturned);
    int totalReturnedItems = 0;
    for (int s = 0; s < _segments; s++) {
        items2Return.insert(items2Return.end(), itemsFromSegments[s].begin(), itemsFromSegments[s].end());
        totalReturnedItems += itemsFromSegments[s].size();
        itemsFromSegments[s].clear();
    }
    assert(items2Return.size() == totalReturnedItems);
    //TODO: Duplicate check
    _functions[0]->sortItems(items2Return, _modalities);

    for (int i = 0; i < k; i++) {
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        cout << "(CTRL) Return item " << items2Return[i].itemId << " " << items2Return[i].aggScore << " "
        << items2Return[i].distance[0] << endl;
#endif
        results.suggs.push_back(items2Return[i].itemId);
    }

    //completedSegments = 0;
    time_point<high_resolution_clock> finish = high_resolution_clock::now();
    results.overheadTime = duration<double, milli>(finish - begin).count();
    return results;
}

template <typename T>
void ExqController<T>::reset_model() {

}

template <typename T>
vector<ExqArray<pair<int, float>>> ExqController<T>::get_descriptors(vector<int> ids, int mod) {
    auto res = vector<ExqArray<pair<int,float>>>(ids.size());
    for (int i = 0; i < (int)ids.size(); i++) {
        res[i] = _functions[mod]->getDescriptorInformation(*_handler->getDescriptor(ids[i],mod));
    }
    return res;
}

template class exq::ExqController<exq::ExqDescriptor<uint64_t,uint64_t,uint64_t>>;


