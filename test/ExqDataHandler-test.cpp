//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "gtest/gtest.h"
#include "ExqDataHandler.h"
#include "ExqR64Descriptor.h"
#include <vector>
#include <string>

using namespace exq;
using std::vector;
using std::string;

class SingleModalityDataHandlerFixture: public ::testing::Test {
public:
    ExqDataHandler* dataHandler;

    SingleModalityDataHandlerFixture() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/singleModTop.h5", "data/singleModIds.h5", "data/singleModRatios.h5"};
        compFiles.push_back(fileNames);
        vector<bool> activeModalities {true};

        ExqDataHandler* dataHandler = new ExqDataHandler(compFiles, 1, activeModalities, 1);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityDataHandlerFixture() {
        delete dataHandler;
    }
};

TEST_F(SingleModalityDataHandlerFixture, firstItem) {
    ExqR64Descriptor* firstItem = dataHandler->items.at(0)->at(0);
    ASSERT_EQ(firstItem->id, 0);
    ASSERT_EQ(firstItem->topFeature, 0);
    ASSERT_EQ(firstItem->featureIds, 0);
    ASSERT_EQ(firstItem->featureRatios, 0);
}

TEST_F(SingleModalityDataHandlerFixture, lastitem) {
    ExqR64Descriptor* lastItem = dataHandler->items.at(0)->at(4);

    ASSERT_EQ(lastItem->id, 4);
    ASSERT_EQ(lastItem->topFeature, 0);
    ASSERT_EQ(lastItem->featureIds, 0);
    ASSERT_EQ(lastItem->featureRatios, 0);
}

TEST_F(SingleModalityDataHandlerFixture, count) {
    ASSERT_EQ(dataHandler->items.at(0)->size(), 5);
}
