//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqController.h"
//#include "../ExqFunctionsR64.h"

#include <future>
#include <utility>

#ifdef WRITE_MODEL
#include <fstream>
#include <iostream>
#endif

using namespace exq;
using std::async;
using std::future_status;
using std::milli;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::cout;
using std::endl;
using std::fstream;


template<class T>
ExqController<T>::ExqController(
        int iota,
        int noms,
        int workers,
        int segments,
        int numberModalities,
        vector<int> modFeatureDimensions,
        int bClusters,
        vector<IExqFunctions<T>*> functions,
        IExqDataHandler<T>* handler,
        vector<ExqClassifier*> classifiers,
        ExqWorker<T>* worker,
        const vector<ItemProperties>& itemProps,
        const vector<vector<Props>>& vidProps,
        vector<double> modWeights,
        bool ffs,
        int guaranteedSlots
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

    _orgModWeights = vector<double>(modWeights);
    _positivesFromMod = vector<double>(_modalities, 0.0);
    for (int m = 0; m < (int)_orgModWeights.size(); m++) {
        _orgWeightSum += _orgModWeights[m];
    }
    _rescaledModWeights = vector<double>(modWeights);
    _modalityWeights = std::move(modWeights);

    _ffs = ffs;
    _guaranteedSlots = guaranteedSlots;

    cout << "(CTRL) Loading data..." << endl;
    // Load data
    _handler->loadData(_modalities);
    cout << "(CTRL) Data loaded" << endl;

    _threads.resize(_numWorkers);
}

template<class T>
ExqController<T>::~ExqController() {
    for (int m = 0; m < _modalities; m++) {
        delete _classifiers[m];
    }
}

template<class T>
vector<double> ExqController<T>::train(const vector<uint32_t>& trainIds, const vector<float>& trainLabels,
                                       bool changeFilters, Filters filters) {
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(CTRL) In train" << endl;
#endif
    vector<double> times = vector<double>();
    vector<vector<double>> weights = vector<vector<double>>(_modalities, vector<double>());
    time_point<high_resolution_clock> begin = high_resolution_clock::now();
    time_point<high_resolution_clock> finish = high_resolution_clock::now();

    cout << "(CTRL) Setting Filters" << endl;
    if (changeFilters) {
        _activeFilters.setFilters(filters);
        //TODO: Reset cache of frc in ECPQOP
    }

    cout << "(CTRL) Training classifier(s)" << endl;
    for (int m = 0; m < _modalities; m++) {
        vector<vector<double>> trainingItems = vector<vector<double>>();
        for (uint32_t trainId : trainIds) {
            IExqDescriptor<T>* desc = _handler->getDescriptor(trainId, m);
            ExqArray<pair<int, float>> descVals = _functions[m]->getDescriptorInformation(*desc);
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
    cout << "Getting clusters" << endl;
    _pq_state = _handler->selectClusters(bPerMod, _classifiers, _activeFilters);
    cout << "Got clusters" << endl;
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    for (int m = 0; m < _modalities; m++) cout << "(CTRL) PQ state( " << m << "): " << _pq_state[m] << endl;
#endif
    finish = high_resolution_clock::now();
    times.push_back(duration<double, milli>(finish - begin).count());
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(CTRL) Training done" << endl;
#endif
#ifdef WRITE_MODEL
    fstream file;
    file.open("svm.txt", ios_base::app);
    for (int m = 0; m < _modalities; m++) {
        file << "[";
        for (int w = 0; w < weights[m].size()-1; w++) {
            file << weights[m][w] << ",";
        }
        file << weights[m][weights[m].size()-1] << "] | " << _classifiers[m]->getBias() << endl;
    }
#endif
    return times;
}


template<class T>
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

    //if (_ffs && _slotsUnset) {
    //    _slots = vector<int>(_modalities, initSlots);
    //    _slotsUnset = false;
    //}
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
                                            usedFilters, _rescaledModWeights, _slots, _ffs);
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
    if (totalItemsReturned != 0) {
        //if (_ffs) {
        //
        //} else {
        auto unique = std::map<uint32_t,int>();
        auto modItems = vector<vector<ExqItem>>(_modalities);
        for (int s = 0; s < _segments; s++) {
            for (int i = 0; i < (int) itemsFromSegments[s].size(); i++) {
                uint32_t itemId = itemsFromSegments[s][i].itemId;
                if (!unique.contains(itemId)) {
                    itemsFromSegments[s][i].aggScore = 0.0;
                    modItems[itemsFromSegments[s][i].fromModality[0]].push_back(itemsFromSegments[s][i]);
                    items2Return.push_back(itemsFromSegments[s][i]);
                    unique[itemId] = items2Return.size()-1;
                } else {
                    for (int j = 0; j < (int) itemsFromSegments[s][i].fromModality.size(); j++) {
                        bool add = true;
                        for (int n = 0; n < (int) items2Return[unique[itemId]].fromModality.size(); n++) {
                            if (items2Return[unique[itemId]].fromModality[n] == itemsFromSegments[s][i].fromModality[j])
                                add = false;
                        }
                        if (add)
                            items2Return[unique[itemId]].fromModality.push_back(itemsFromSegments[s][i].fromModality[j]);
                    }
                }
            }
            //items2Return.insert(items2Return.end(), itemsFromSegments[s].begin(), itemsFromSegments[s].end());
            //itemsFromSegments[s].clear();
        }

        _functions[0]->sortItems(items2Return, _modalities, _rescaledModWeights, true);

#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        cout << "Size of items2Return: " << items2Return.size() << endl;
        if (items2Return.size() > 0 && _modalities > 1) {
            cout << items2Return[0].modRank.size() << endl;
            for (int m = 0; m < _modalities; m++)
                cout << items2Return[0].modRank[m] << endl;
            cout << items2Return[0].aggScore << endl;
        }
#endif
        //}

        unique.clear();
        if ((_guaranteedSlots*_modalities) != k) {
            for (int i = 0; i < (int) items2Return.size(); i++) {
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
                cout << "(CTRL) Return item " << items2Return[i].itemId << " " << items2Return[i].aggScore << " "
                << items2Return[i].distance[0] << endl;
#endif
                unique[items2Return[i].itemId] = 1;
                results.suggs.push_back(items2Return[i].itemId);
                if ((int) results.suggs.size() == k - (_guaranteedSlots * _modalities) ||
                    (int) results.suggs.size() == k)
                    break;
            }
        }

        if (_guaranteedSlots != 0) {
            for (int m = 0; m < _modalities; m++) {
                std::sort(modItems[m].begin(), modItems[m].end(),
                          [m](const ExqItem &lhs, const ExqItem &rhs) {
                              return lhs.distance[m] > rhs.distance[m];
                          });
                int inserted = 0;
                for (int i = 0; i < (int) modItems[m].size(); i++) {
                    if (!unique.contains(modItems[m][i].itemId)) {
                        results.suggs.push_back(modItems[m][i].itemId);//items2Return.push_back(modItems[m][i]);
                        inserted++;
                        if (inserted == _guaranteedSlots) break;
                    }
                }
            }
        }
    }

    // Update session clusters counts
    _handler->updateSessionInfo(results.suggs);

    if ((int)results.suggs.size() < k) {
        cout << "Incremental Retrieval" << endl;
        auto bPerMod = vector<int>(_modalities);
        bool done = true;
        for (int m = 0; m < _modalities; m++) {
            if (_pq_state[m])
                bPerMod[m] = 0;
            else {
                cout << "mod " << m << " pq_state " << _pq_state[m] << endl;
                done = false;
                bPerMod[m] = _bClusters;
            }
        }
        if (!done) {
            cout << "In !done" << endl;
            _pq_state = _handler->selectClusters(bPerMod, _classifiers, _activeFilters, true);
            results = suggest(k, seenItems, changeFilters, filters, results);
        }
    }
    //completedSegments = 0;
    time_point<high_resolution_clock> finish = high_resolution_clock::now();
    results.overheadTime = duration<double, milli>(finish - begin).count();
    return results;
}

template<class T>
void ExqController<T>::reset_model(bool resetSession) {
    for (int m = 0; m < _modalities; m++)
        _classifiers[m]->resetClassifier();
    _activeFilters = ItemFilter();
    if (resetSession) {
        _handler->resetSessionInfo();
    }
}

template<class T>
bool ExqController<T>::update_modality_weights(vector<uint32_t>& ids, vector<float>& labels) {
    if (_ffs) {
        auto nSuggs = (float) _retSuggsFFS.size();
        if (nSuggs == 0) return true; // Sanity check, if no suggestions were returned, there is nothing more to show

        auto modPositives = vector<int>(_modalities, 0);
        for (int i = 0; i < (int) ids.size(); i++) {
            if (_retSuggsFFS.contains(ids[i])) {
                if (labels[i] == 1.0) modPositives[_retSuggsFFS[ids[i]].first]++;
            }
        }
        int mx = -INT32_MAX;
        int mx_mod = -1;
        for (int m = 0; m < _modalities; m++) {
            if (mx < modPositives[m]) {
                mx = modPositives[m];
                mx_mod = m;
            }
        }
        bool check = false;
        auto mn_checked = unordered_set<int>();
        mn_checked.insert(mx_mod); // Add max modality as it doesn't need to be checked
        //for (int m = 0; m < _modalities; m++) {
        //    cout << "Current: Mod " << m << " has " << _slots[m] << " slots, round positives = " << modPositives[m]
        //    << " | " << mx_mod << " | " << mx << endl;
        //}
        while (!check) {
            int mn = INT32_MAX;
            int mn_mod = -1;
            for (int m = 0; m < _modalities; m++) {
                if (mn_checked.contains(m)) continue;
                if (mn > modPositives[m]) {
                    mn = modPositives[m];
                    mn_mod = m;
                }
            }
            // Check if there is a difference
            if (mx != mn) {
                // Add 1 slot to max and reduce 1 slot from min if min has fight slots
                if (_slots[mn_mod] > _guaranteedSlots) {
                    _slots[mn_mod] -= 1;
                    _slots[mx_mod] += 1;
                    check = true;
                }
            }
            mn_checked.insert(mn_mod);
            if ((int)mn_checked.size() == _modalities) check = true;
        }
        for (int m = 0; m < _modalities; m++) {
            cout << "Updated: Mod " << m << " has " << _slots[m] << " slots, round positives = " << modPositives[m]
            << " | " << mx_mod << " | " << mx << endl;
        }

        _retSuggsFFS.clear();
        return true;
    }

    auto nSuggs = (float) _retSuggs.size();
    if (nSuggs == 0) return true; // Sanity check, if no suggestions were returned, there is nothing more to show

    auto modChange = vector<double>(_modalityWeights.size(),0.0);
    //auto modPNcnt= vector<pair<float,float>>(_modalities, std::make_pair(0,0));

    float rel = 0;
    bool found = false;
    for (int i = 0; i < (int) ids.size(); i++) {
        if (_retSuggs.contains(ids[i])) {
            found = true;
            vector<int> fromMod = _retSuggs[ids[i]].second;
            //if (labels[i] == 1.0) {
            //    rel += 1;
            //for (int j = 0; j < (int) fromMod.size(); j++) {
                    // _positivesFromMod[fromMod[j]] += 1;
                    // modPNcnt[fromMod[j]].first += 1;
                    // MPDBL
                    // modChange[fromMod[j]] += _retSuggs[ids[i]].first[fromMod[j]] * 2;
                //}
                // modPNcnt[fromMod].first += 1;
                // MPDBL
                // modChange[fromMod] += _retSuggs[ids[i]].first[fromMod] * 2;
                // } else {
                //      for (int j = 0; j < (int) fromMod.size(); j++) {
                //          modChange[fromMod[j]] += _retSuggs[ids[i]].first[fromMod[j]];
                //          modPNcnt[fromMod[j]].second += 1;
                //      }
                // }
            //}

            // NN/MDBL
            if (labels[i] == 1.0) {
                rel += 1;
                for (int m = 0; m < _modalities; m++) {
                    modChange[m] += _retSuggs[ids[i]].first[m];
                    for (int j = 0; j < (int) fromMod.size(); j++) {
                        if (m == fromMod[j]) {
                            modChange[m] += _retSuggs[ids[i]].first[m];
                            _positivesFromMod[fromMod[j]] += 1;
                        }
                    }
                }
            }

            // PDBL
            //for (int m = 0; m < _modalities; m++) {
                //if (labels[i] == 1.0) {
                    //modChange[m] += _retSuggs[ids[i]].first[m];
                    //modChange[m] += _retSuggs[ids[i]].first[m] * 2;
                //} else {
                //    modChange[m] += _retSuggs[ids[i]].first[m];
                //}
            //}
        }
    }
    _numPositives += rel;
    _update_cnt++; // Number of times update weights has been called

    //if (_update_cnt > _guaranteedSlots && _numPositives > 0) {
        cout << "_numPositives: " << _numPositives;
        for (int m = 0; m < _modalities; m++) {
            cout << " | Mod " << m << ": " << _positivesFromMod[m];
        }
        cout << endl;
        //_update_cnt = 0;
        _numPositives = 0;
        //vector<int> I(_modalities);
        //std::iota(I.begin(), I.end(),0);
        //sort(I.begin(), I.end(), [&](int i, int j) { return _positivesFromMod[i] > _positivesFromMod[j]; });

        //double val = _positivesFromMod[I[0]];
        //_modalityWeights[I[0]] = _modalities;
        //double weight = _modalities;
        //for (int m = 1; m < (int) I.size(); m++) {
        //    if (_positivesFromMod[I[m]] == val) {
        //        _modalityWeights[I[m]] = weight;
        //    } else {
        //        val = _positivesFromMod[I[m]];
        //        weight = _modalities - m;
        //        _modalityWeights[I[m]] = weight;
        //    }
        //}
        //for (int m = 0; m < _modalities; m++) {
        //_modalityWeights[m] += modChange[m];
        // PN count ratio
        //if (rel != 0.0)
        //    _modalityWeights[m] += modPNcnt[m].first/rel;
        //else
        //    _modalityWeights[m] += (nSuggs-modPNcnt[m].second)/nSuggs;
        //}

        // Rescale weights into original sum weight
        double msum = 0.0;
        for (int m = 0; m < _modalities; m++) {
            _positivesFromMod[m] = 0;
            msum += _modalityWeights[m];
        }

        for (int m = 0; m < _modalities; m++) {
            _rescaledModWeights[m] = _modalityWeights[m] * (_orgWeightSum / msum);
        }

        if (found) {
            cout << "Updated modality weights: [";
            for (int m = 0; m < _modalities; m++) {
                cout << " (" << _modalityWeights[m] << "," << _rescaledModWeights[m] << ") ";
            }
            cout << "]" << endl;
        }
    //}

    _retSuggs.clear();
    return true;
}


template<class T>
void ExqController<T>::reset_modality_weights() {
    for (int m = 0; m < _modalities; m++) {
        _modalityWeights[m] = _orgModWeights[m];
        _rescaledModWeights[m] = _orgModWeights[m];
        _update_cnt = 0;
        _numPositives = 0;
    }
    if (_ffs) {
        _slotsUnset = true;
        _slots.clear();
    }
    //cout << "Weights reset: [" <<
    //     _modalityWeights[0] << ", " << _modalityWeights[1] << ", " << _modalityWeights[2] << "]" << endl;
}

template<class T>
vector<ExqArray<pair<int, float>>> ExqController<T>::get_descriptors(vector<int> ids, int mod) {
    auto res = vector<ExqArray<pair<int,float>>>(ids.size());
    for (int i = 0; i < (int)ids.size(); i++) {
        res[i] = _functions[mod]->getDescriptorInformation(*_handler->getDescriptor(ids[i],mod));
    }
    return res;
}

template class exq::ExqController<uint64_t>;
