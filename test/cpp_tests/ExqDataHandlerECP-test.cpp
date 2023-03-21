//
// Created by Omar Shahbaz Khan on 21/03/2023.
//

#include "gtest/gtest.h"
#include "base/IExqFunctions.h"
#include "base/r64/ExqDataHandlerECP.h"
#include "base/r64/ExqFunctionsR64.h"
#include "base/Metadata/ItemProperties.h"

using exq::ExqDataHandlerECP;
using exq::IExqFunctions;
using exq::ExqFunctionsR64;
using exq::ItemProperties;
using exq::Props;
using exq::ExpansionType;

using std::vector;
using std::string;

class SingleModalityDataHandlerECPFixture: public ::testing::Test {
public:
    ExqDataHandlerECP* dataHandler;

    SingleModalityDataHandlerECPFixture() {
        vector<string> fileNames {"data/yfcc/vis_index_full.cnfg"};
        vector<bool> activeModalities {true};
        vector<int> featureDimensions = {1000};
        vector<ItemProperties> itemProps = vector<ItemProperties>();
        vector<vector<Props>> vidProps = vector<vector<Props>>();

        auto functions = vector<IExqFunctions<uint64_t>*>(1);
        uint64_t bit_shift_t = 54; // Bits representing the value of the top feature
        double multiplier_t = 10000000000000000.0; // Divisor for the decompression of the value
        uint64_t decomp_mask_t = 18014398509481983; // Mask used to compress id and value of the top feature
        int n_feat_int = 6; // Number of additional features
        int bit_shift_ir = 10; // Bits representing the ids and values of the additional features
        uint64_t decomp_mask_ir = 1023; // Mask for the ids and values of the additional features
        double multiplier_ir = 1000; // Divisor for the values of the additional features
        int total_feat = 1000; // total number of features in 
        functions[0] = new ExqFunctionsR64(n_feat_int+1, 1, bit_shift_t, bit_shift_ir, bit_shift_ir, 
                                           decomp_mask_t, multiplier_t, decomp_mask_ir, decomp_mask_ir,
                                           multiplier_ir);

        this->dataHandler = new ExqDataHandlerECP(fileNames, 1, functions, featureDimensions,
                                                  itemProps, vidProps, ExpansionType::ORIGINAL_CNT, 0);
    }

    void SetUp() {}

    void TearDown() {}

    ~SingleModalityDataHandlerECPFixture() {
        delete dataHandler;
    }
};

TEST_F(SingleModalityDataHandlerECPFixture, loadData) {
    this->dataHandler->loadData(1);
}