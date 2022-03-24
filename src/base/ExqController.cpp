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
        const vector<vector<Props>>& vidProps,
        vector<double> modWeights
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

    _learningRate = modWeights[0] * 0.1;
    _orgModWeights = vector<double>(modWeights);
    _normalizedModWeights = vector<double>(modWeights);
    _modalityWeights = std::move(modWeights);

    cout << "(CTRL) Loading data..." << endl;
    // Load data
    _handler->loadData(_modalities);
    cout << "(CTRL) Data loaded" << endl;

    _threads.resize(_numWorkers);
}

template <typename T>
ExqController<T>::~ExqController() {
    for (int m = 0; m < _modalities; m++) {
        delete _classifiers[m];
    }
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
        //TODO: Reset cache of frc in ECPQOP

    for (int m = 0; m < _modalities; m++) {
        vector<vector<double>> trainingItems = vector<vector<double>>();
        for (uint32_t trainId : trainIds) {
            T desc = _handler->getDescriptor(trainId, m);
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
    auto bPerMod = vector<int>(_modalities);
    //vector<double> bias;
for (int m = 0; m < _modalities; m++) {
        bPerMod[m] = _bClusters;
        //bias.push_back(_classifiers[m]->getBias());
    }
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(CTRL) Selecting clusters" << endl;
#endif
    //_handler->selectClusters(bPerMod, weights, bias, _activeFilters);
    _pq_state = _handler->selectClusters(bPerMod, _classifiers, _activeFilters);
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    for (int m = 0; m < _modalities; m++) cout << "(CTRL) PQ state( " << m << "): " << _pq_state[m] << endl;
#endif
    finish = high_resolution_clock::now();
    times.push_back(duration<double, milli>(finish - begin).count());
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(CTRL) Training done" << endl;
#endif
    return times;
}


template <typename T>
TopResults ExqController<T>::suggest(int k, const vector<uint32_t>& seenItems, bool changeFilters,
                                     const Filters& filters, TopResults prevResults) {
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(CTRL) Setting suggest parameters" << endl;
#endif
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    auto results = TopResults(_segments);
    int completedSegments = 0;
    int runningSegments = 0;
    vector<int> workerSegments = vector<int>(_numWorkers, -1);
    std::fill(workerSegments.begin(), workerSegments.end(), -1);
    auto seenSet = unordered_set<uint32_t>();
    for (const uint32_t& seenItem : seenItems) {
        seenSet.insert(seenItem);
    }
    auto itemsFromSegments = vector<vector<ExqItem>>(_segments);
    std::fill(itemsFromSegments.begin(), itemsFromSegments.end(), vector<ExqItem>());
    int totalItemsReturned = 0;

    ItemFilter usedFilters;
    changeFilters ? usedFilters = ItemFilter(filters) : usedFilters = _activeFilters;

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(CTRL) Starting workers" << endl;
#endif
    while (completedSegments < _segments) {
        for (int w = 0; w < _numWorkers; w++) {
            if (workerSegments[w] == -1 && runningSegments < _segments) {
                int currSegment = runningSegments; // Avoids race condition
                workerSegments[w] = runningSegments;
                itemsFromSegments[currSegment] = vector<ExqItem>();
                _threads[w] = async(std::launch::async, [&] {
                    return _worker->suggest(k, itemsFromSegments[currSegment],
                                            _classifiers,
                                            currSegment, _segments,
                                            _noms, _modalities, _handler, _functions, seenSet,
                                            results.totalTimePerSegment[currSegment],
                                            results.totalItemsConsideredPerSegment[currSegment], w,
                                            usedFilters, _normalizedModWeights);
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
                totalItemsReturned += (int)itemsFromSegments[workerSegments[w]].size();
                workerSegments[w] = -1;
                completedSegments++;
            }
        }
    }
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(CTRL) Workers done" << endl;
#endif
    if (!prevResults.suggs.empty()) {
        results.suggs = prevResults.suggs;
    }
    auto items2Return = vector<ExqItem>();
    items2Return.reserve(totalItemsReturned);
    auto unique = unordered_set<uint32_t>();
    if (totalItemsReturned != 0) {
        for (int s = 0; s < _segments; s++) {
            for (int i = 0; i < (int)itemsFromSegments[s].size(); i++) {
                if (!unique.contains(itemsFromSegments[s][i].itemId)) {
                    itemsFromSegments[s][i].aggScore = 0.0;
                    items2Return.push_back(itemsFromSegments[s][i]);
                    unique.insert(itemsFromSegments[s][i].itemId);
                }
            }
            //items2Return.insert(items2Return.end(), itemsFromSegments[s].begin(), itemsFromSegments[s].end());
            itemsFromSegments[s].clear();
        }
        _functions[0]->sortItems(items2Return, _modalities, _normalizedModWeights, true);

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        cout << "Size of items2Return: " << items2Return.size() << endl;
        if (items2Return.size() > 0 && _modalities > 1) {
            cout << items2Return[0].modRank.size() << endl;
            for (int m = 0; m < _modalities; m++)
                cout << items2Return[0].modRank[m] << endl;
            cout << items2Return[0].aggScore << endl;
        }
#endif
        for (int i = 0; i < (int)items2Return.size(); i++) {
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
            cout << "(CTRL) Return item " << items2Return[i].itemId << " " << items2Return[i].aggScore << " "
            << items2Return[i].distance[0] << endl;
#endif
            results.suggs.push_back(items2Return[i].itemId);
            _retSuggs[items2Return[i].itemId] = vector<double>(items2Return[i].modRank);
            if ((int)results.suggs.size() == k) break;
        }
    }

    //TODO: Update session clusters in ECPQOP

    if ((int)results.suggs.size() < k) {
        //cout << "Incremental Retrieval" << endl;
        auto bPerMod = vector<int>(_modalities);
        bool done = true;
        for (int m = 0; m < _modalities; m++) {
            if (_pq_state[m])
                bPerMod[m] = 0;
            else {
                //cout << "mod " << m << " pq_state " << _pq_state[m] << endl;
                done = false;
                bPerMod[m] = _bClusters;
            }
        }
        if (!done) {
            //cout << "In !done" << endl;
            _pq_state = _handler->selectClusters(bPerMod, _classifiers, _activeFilters, true);
            results = suggest(k, seenItems, changeFilters, filters, results);
        }
    }
    //completedSegments = 0;
    time_point<high_resolution_clock> finish = high_resolution_clock::now();
    results.overheadTime = duration<double, milli>(finish - begin).count();
    return results;
}

template <typename T>
void ExqController<T>::reset_model() {
    for (int m = 0; m < _modalities; m++)
        _classifiers[m]->resetClassifier();
}

template <typename T>
void ExqController<T>::update_modality_weights(vector<uint32_t>& ids, vector<float>& labels) {
    auto nSuggs = (float) _retSuggs.size();
    if (nSuggs == 0) return;

    float rel = 0;
    bool found = false;
    bool norm = false;
    for (int i = 0; i < (int) ids.size(); i++) {
        if (_retSuggs.contains(ids[i])) {
            found = true;
            if (labels[i] == 1.0) rel += 1;
            for (int m = 0; m < _modalities; m++) {
                //_modalityWeights[m] += (((labels[i]/nSuggs) * _retSuggs[ids[i]][m])/_change);
                _modalityWeights[m] += (((labels[i]/nSuggs) * _retSuggs[ids[i]][m])/_change) * _learningRate;
                if (_modalityWeights[m] < 0.0) {
                    norm = true;
                }
                //if (_modalityWeights[m] > 1.0) {
                //    double max = std::max_element(_modalityWeights.begin(), _modalityWeights.end())[0];
                //    double min = std::min_element(_modalityWeights.begin(), _modalityWeights.end())[0];
                //    for (int mm = 0; mm < _modalities; mm++) {
                //        _normalizedModWeights[mm] = _modalityWeights[mm]/(max+abs(min));
                //    }
                //}
            }
        }
    }
    if (norm) {
        int a = 0;
        int b = 1;
        double max = std::max_element(_modalityWeights.begin(), _modalityWeights.end())[0];
        double min = std::min_element(_modalityWeights.begin(), _modalityWeights.end())[0];
        for (int m = 0; m < _modalities; m++) {
            _normalizedModWeights[m] = (b-a)*(_modalityWeights[m]-min)/(max-min)+a;
        }
    } else {
        _normalizedModWeights = vector<double>(_modalityWeights);
    }
    if (found)  {
        cout << "Updated modality weights: [" <<
        _modalityWeights[0] << ", " << _modalityWeights[1] << ", " << _modalityWeights[2] << "], " <<
        _normalizedModWeights[0] << ", " << _normalizedModWeights[1] << ", " << _normalizedModWeights[2] << "]" << endl;
        if (rel > 0) {
            // Decrease rate based on relevant items and suggestions returned
            _learningRate *= (rel/nSuggs);
        } else {
            // Increase rate based on the number of items suggested and rounds no items were judged
            _learningRate += _orgModWeights[0] * 0.01;
        }
        _change = 1.0;
    }
    else {
        _change += 1.0;
    }
    _retSuggs.clear();
}

template <typename T>
void ExqController<T>::reset_modality_weights() {
    for (int m = 0; m < _modalities; m++) {
        _modalityWeights[m] = _orgModWeights[m];
        _normalizedModWeights[m] = _orgModWeights[m];
    }
    _learningRate = _orgModWeights[0] * 0.5;
    //cout << "Weights reset: [" <<
    //     _modalityWeights[0] << ", " << _modalityWeights[1] << ", " << _modalityWeights[2] << "]" << endl;
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


