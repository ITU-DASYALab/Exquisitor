//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "gtest/gtest.h"
#include "ExqDataHandler.h"
#include "ExqDataHandlerH5.h"
#include <vector>
#include <string>

using namespace exq;
using std::vector;
using std::string;
using std::cout;
using std::endl;

class SingleModalityDataHandlerFixture: public ::testing::Test {
public:
    ExqDataHandlerH5* dataHandler;

    SingleModalityDataHandlerFixture() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/random_5_10/top.h5", "data/random_5_10/ids.h5", "data/random_5_10/ratios.h5"};
        compFiles[0] = fileNames;
        vector<bool> activeModalities {true};

        this->dataHandler = new ExqDataHandlerH5(compFiles, 1, activeModalities, 1);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityDataHandlerFixture() {
        delete dataHandler;
    }
};

TEST_F(SingleModalityDataHandlerFixture, firstItemRead) {
    auto firstItem = this->dataHandler->getItem(0);
    ASSERT_EQ(firstItem->getId(), 0);
    ASSERT_EQ(firstItem->getTop(), 2533274790396757);
    ASSERT_EQ(firstItem->getFeatureIds(), 1125912792203265);
    ASSERT_EQ(firstItem->getFeatureRatios(), 242353128979235512);

    cout << "TEST firstItemRead in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}

TEST_F(SingleModalityDataHandlerFixture, lastitemRead) {
    auto lastItem = dataHandler->getItem(4, 0);

    ASSERT_EQ(lastItem->getId(), 4);
    ASSERT_EQ(lastItem->getTop(), 281474976711517);
    ASSERT_EQ(lastItem->getFeatureIds(), 1407404948324356);
    ASSERT_EQ(lastItem->getFeatureRatios(), 255864967252411121);

    cout << "TEST lastItemRead in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}

TEST_F(SingleModalityDataHandlerFixture, count) {
    ASSERT_EQ(dataHandler->getTotalItemsCount(0), 5);

    cout << "TEST count in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}

TEST_F(SingleModalityDataHandlerFixture, inheritance) {
    ExqDataHandler<uint64_t,uint64_t,uint64_t>* dh = dataHandler;

    ASSERT_EQ(dh->getItem(0)->getId(), 0);

    cout << "TEST inheritance in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}
