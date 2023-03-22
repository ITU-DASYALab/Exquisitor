//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "gtest/gtest.h"
#include "base/IExqDataHandler.h"
#include "base/r64/ExqDataHandlerH5.h"
#include <vector>
#include <string>

using namespace exq;
using std::vector;
using std::string;
using std::cout;
using std::endl;

class SingleModalityDataHandlerH5Fixture: public ::testing::Test {
public:
    ExqDataHandlerH5* dataHandler;

    SingleModalityDataHandlerH5Fixture() {
        vector<vector<string>> compFiles(1);
        vector<string> fileNames {"data/random_5_10/top.h5", "data/random_5_10/ids.h5", "data/random_5_10/ratios.h5"};
        compFiles[0] = fileNames;
        vector<bool> activeModalities {true};

        this->dataHandler = new ExqDataHandlerH5(compFiles, 1);
        this->dataHandler->loadData(1);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityDataHandlerH5Fixture() {
        delete dataHandler;
    }
};

TEST_F(SingleModalityDataHandlerH5Fixture, firstItemRead) {
    auto firstItem = this->dataHandler->getDescriptor(0);
    ExqArray<uint64_t>* ids = firstItem->getFeatureIds();
    ExqArray<uint64_t>* ratios = firstItem->getFeatureRatios();

    ASSERT_EQ(firstItem->getId(), 0);
    ASSERT_EQ(*firstItem->getTop(), 2533274790396757);
    ASSERT_EQ(ids->getItem(0), 1125912792203265);
    ASSERT_EQ(ratios->getItem(0), 242353128979235512);

    cout << "TEST firstItemRead in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}

TEST_F(SingleModalityDataHandlerH5Fixture, lastitemRead) {
    auto lastItem = dataHandler->getDescriptor(4, 0);
    ExqArray<uint64_t>* ids = lastItem->getFeatureIds();
    ExqArray<uint64_t>* ratios = lastItem->getFeatureRatios();
 
    ASSERT_EQ(lastItem->getId(), 4);
    ASSERT_EQ(*lastItem->getTop(), 281474976711517);
    ASSERT_EQ(ids->getItem(0), 1407404948324356);
    ASSERT_EQ(ratios->getItem(0), 255864967252411121);

    cout << "TEST lastItemRead in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}

TEST_F(SingleModalityDataHandlerH5Fixture, count) {
    ASSERT_EQ(dataHandler->getTotalItemsCount(0), 5);

    cout << "TEST count in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}

TEST_F(SingleModalityDataHandlerH5Fixture, inheritance) {
    IExqDataHandler<uint64_t>* dh = dataHandler;

    ASSERT_EQ(dh->getDescriptor(0)->getId(), 0);

    cout << "TEST inheritance in SingleModalityDataHandlerFixture SUCCEEDED!" << endl;
}
