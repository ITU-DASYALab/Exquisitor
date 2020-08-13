//
// Created by Omar Shahbaz Khan on 07/07/2020.
//

#include "gtest/gtest.h"
#include "ExqWorker.h"
#include "ExqFunctionsR64.h"
#include "ExqDataHandlerH5.h"
#include "ExqClassifier.h"
#include "ExqDescriptor.h"

using std::vector;
using std::string;
using namespace exq;

class SingleModalityWorker: public ::testing::Test {
public:
    ExqDataHandler<ExqDescriptor<uint64_t,uint64_t,uint64_t>>* _dataHandler;
    ExqFunctions<ExqDescriptor<uint64_t,uint64_t,uint64_t>>* _functionsR64;
    ExqClassifier* _classifier;
    ExqWorker<ExqDescriptor<uint64_t,uint64_t,uint64_t>>* _worker;
    vector<double> _weights;

    //TODO: Initialize all mock object and values to test suggest
    SingleModalityWorker() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/random_5_10/top.h5", "data/random_5_10/ids.h5", "data/random_5_10/ratios.h5"};
        compFiles[0] = fileNames;
        vector<bool> activeModalities {true};

        _dataHandler = new ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>(compFiles, 1);
        _dataHandler->loadData(activeModalities.size(), 1);
        _functionsR64 = new ExqFunctionsR64<uint64_t, uint64_t, uint64_t>(5, 48, 16, 16, 1000, 1000);
        _classifier = new ExqClassifier();
        _worker = new ExqWorker<ExqDescriptor<uint64_t,uint64_t,uint64_t>>();

        //_weights = _classifier->train(trainingItems, trainingItemLabels);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityWorker() {
        delete _classifier;
    }
};

TEST_F(SingleModalityWorker, get_k_stuff) {
    int k;
    vector<ExqItem> items;
    unordered_set<uint32_t> seenItems = unordered_set<uint32_t>();
    _worker->suggest(k,items,_weights,_classifier->getBias(), 0, 2, 10, 1, _dataHandler, _functionsR64, seenItems);
    assert(items.size() == k);
}
