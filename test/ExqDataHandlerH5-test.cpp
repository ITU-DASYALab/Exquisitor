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
    ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>* dataHandler;

    SingleModalityDataHandlerFixture() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/random_5_10/top.h5", "data/random_5_10/ids.h5", "data/random_5_10/ratios.h5"};
        compFiles[0] = fileNames;
        vector<bool> activeModalities {true};

        this->dataHandler = new ExqDataHandlerH5<uint64_t,uint64_t,uint64_t>(compFiles, 1);
        this->dataHandler->loadData(activeModalities.size(), 1);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityDataHandlerFixture() {
        delete dataHandler;
    }
};

TEST_F(SingleModalityDataHandlerFixture, firstItemRead) {
    auto firstItem = this->dataHandler->getDescriptor(0);
    ASSERT_EQ(firstItem->getId(), 0);
    ASSERT_EQ(firstItem->getTop(), 2533274790396757);
    ASSERT_EQ(firstItem->getFeatureIds(), 1125912792203265);
    ASSERT_EQ(firstItem->getFeatureRatios(), 242353128979235512);

    cout << "TEST firstItemRead in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}

TEST_F(SingleModalityDataHandlerFixture, lastitemRead) {
    auto lastItem = dataHandler->getDescriptor(4, 0);

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
    ExqDataHandler<ExqDescriptor<uint64_t,uint64_t,uint64_t>>* dh = dataHandler;

    ASSERT_EQ(dh->getDescriptor(0)->getId(), 0);

    cout << "TEST inheritance in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}
