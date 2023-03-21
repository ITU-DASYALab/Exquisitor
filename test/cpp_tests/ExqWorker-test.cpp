//
// Created by Omar Shahbaz Khan on 07/07/2020.
//

#include "gtest/gtest.h"
#include "base/ExqWorker.h"
#include "base/r64/ExqFunctionsR64.h"
#include "base/r64/ExqDataHandlerH5.h"
#include "base/ExqClassifier.h"
#include "base/IExqDescriptor.h"

using std::vector;
using std::string;
using namespace exq;

class SingleModalityWorker: public ::testing::Test {
public:
    IExqDataHandler<uint64_t>* _dataHandler;
    vector<IExqFunctions<uint64_t>*> _functionsR64 = {};
    vector<ExqClassifier*> _classifier = {};
    ExqWorker<uint64_t>* _worker;
    vector<double> _weights;

    //TODO: Initialize all mock object and values to test suggest
    SingleModalityWorker() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/random_5_10/top.h5", "data/random_5_10/ids.h5", "data/random_5_10/ratios.h5"};
        compFiles[0] = fileNames;
        vector<bool> activeModalities {true};

        _dataHandler = new ExqDataHandlerH5(compFiles, 1);
        _dataHandler->loadData(activeModalities.size());
        _functionsR64.push_back(new ExqFunctionsR64(5, 1, 48, 16, 16, 1000, 1000));
        _classifier.push_back(new ExqClassifier(12988));
        _worker = new ExqWorker<uint64_t>();

        //_weights = _classifier->train(trainingItems, trainingItemLabels);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityWorker() {
        delete _classifier[0];
    }
};

TEST_F(SingleModalityWorker, get_k_stuff) {
    int k;
    vector<ExqItem> items;
    unordered_set<uint32_t> seenItems = unordered_set<uint32_t>();
    vector<double> times;
    vector<int> totalItemsConsidered;
    vector<double> modWeights;
    vector<int> slots;
    bool ffs = false;
    ItemFilter filter = ItemFilter();
    _worker->suggest(k,items,_classifier, 0, 2, 10, 1,
                     _dataHandler, _functionsR64, seenItems, times[0], totalItemsConsidered[0], 0, filter,
                     modWeights, slots, ffs);
    assert(items.size() == k);
}
