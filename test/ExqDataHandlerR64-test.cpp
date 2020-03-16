//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "gtest/gtest.h"
#include "ExqDataHandlerR64.h"
#include "ExqDataHandlerR64.cpp"
#include <vector>
#include <string>

using namespace exq;
using std::vector;
using std::string;

class SingleModalityDataHandlerFixture: public ::testing::Test {
public:
    ExqDataHandlerR64* dataHandler;

    SingleModalityDataHandlerFixture() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/singleModTop.h5", "data/singleModIds.h5", "data/singleModRatios.h5"};
        compFiles[0] = fileNames;
        vector<bool> activeModalities {true};

        this->dataHandler = new ExqDataHandlerR64(compFiles, 1, activeModalities, 1);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityDataHandlerFixture() {
        delete dataHandler;
    }
};

TEST_F(SingleModalityDataHandlerFixture, firstItem) {
    auto firstItem = this->dataHandler->getItems()->at(0)->at(0);
    ASSERT_EQ(firstItem->getId(), 0);
    ASSERT_EQ(firstItem->getTop(), 0);
    ASSERT_EQ(firstItem->getFeatureIds(), 0);
    ASSERT_EQ(firstItem->getFeatureRatios(), 0);
}

TEST_F(SingleModalityDataHandlerFixture, lastitem) {
    auto lastItem = dataHandler->getItems()->at(0)->at(4);

    ASSERT_EQ(lastItem->getId(), 4);
    ASSERT_EQ(lastItem->getTop(), 0);
    ASSERT_EQ(lastItem->getFeatureIds(), 0);
    ASSERT_EQ(lastItem->getFeatureRatios(), 0);
}

TEST_F(SingleModalityDataHandlerFixture, count) {
    ASSERT_EQ(dataHandler->getItems()->at(0)->size(), 5);
}
