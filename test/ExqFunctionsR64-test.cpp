//
// Created by Omar Shahbaz Khan on 15/03/2020.
//

#include "gtest/gtest.h"
#include "ExqFunctions.h"
#include "ExqFunctionsR64.h"
#include "ExqDataHandlerH5.h"

#include <cmath>

using namespace exq;
using std::vector;
using std::string;
using std::cout;
using std::endl;


class FunctionsR64Fixture: public ::testing::Test {
public:
    ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>* dataHandler;
    ExqFunctionsR64<uint64_t,uint64_t,uint64_t>* functionsR64;

    FunctionsR64Fixture() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/random_5_10/top.h5", "data/random_5_10/ids.h5", "data/random_5_10/ratios.h5"};
        compFiles[0] = fileNames;
        vector<bool> activeModalities {true};

        this->dataHandler = new ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>(compFiles, 1);
        this->dataHandler->loadData(activeModalities.size(),1);
        this->functionsR64 = new ExqFunctionsR64<uint64_t,uint64_t,uint64_t>(5, 48, 16, 16, 1000, 1000);
    }

    void SetUp() {}

    void TearDown() {}

    ~FunctionsR64Fixture() {
        delete functionsR64;
        delete dataHandler;
    }
};

TEST_F(FunctionsR64Fixture, getTopFeatIdForFirstItemR64) {
    auto firstItemDesc = this->functionsR64->getDescriptorInformation(*this->dataHandler->getDescriptor(0));
    auto topFeature = firstItemDesc.getItem(0);

    ASSERT_EQ(topFeature.first, 9);

    cout << "TEST getTopFeatIdForFirstItemR64 in FunctionsR64Fixture SUCCEEDED!" << endl;
}

TEST_F(FunctionsR64Fixture, getTopFeatValueForFirstItemR64) {
    auto firstItemDesc = this->functionsR64->getDescriptorInformation(*this->dataHandler->getDescriptor(0));
    auto topFeature = firstItemDesc.getItem(0);

    ASSERT_FLOAT_EQ(topFeature.second, 0.853);

    cout << "TEST getTopFeatValueForFirstItemR64 in FunctionsR64Fixture SUCCEEDED!" << endl;
}

TEST_F(FunctionsR64Fixture, getFeatIdsForFirstItemR64) {
    auto firstItemDesc = this->functionsR64->getDescriptorInformation(*this->dataHandler->getDescriptor(0));
    int correctIds[4] = {1, 7, 3, 4};
    for (int i = 1; i < 5; i++) {
        ASSERT_EQ(firstItemDesc.getItem(i).first, correctIds[i-1]);
    }

    cout << "TEST getFeatIdsForFirstItemR64 in FunctionsR64Fixture SUCCEEDED!" << endl;
}

TEST_F(FunctionsR64Fixture, inheritance) {
    ExqFunctions<ExqDescriptor<uint64_t,uint64_t,uint64_t>>* f = this->functionsR64;
    auto firstItemDesc = f->getDescriptorInformation(*this->dataHandler->getDescriptor(0));
    int correctIds[4] = {1, 7, 3, 4};
    for (int i = 1; i < 5; i++) {
        ASSERT_EQ(firstItemDesc.getItem(i).first, correctIds[i-1]);
    }
    cout << "TEST getFeatIdsForFirstItemR64 in FunctionsR64Fixture SUCCEEDED!" << endl;
}
