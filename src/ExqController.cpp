//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqController.h"
#include "ExqFunctionsR64.h"

using namespace exq;

ExqController::ExqController(
        int iota,
        int noms,
        int workers,
        int segments,
        int numberModalities,
        int modalities,
        vector<int> modFeatureDimensions,
        vector<vector<string>> compFiles,
        vector<string> cnfgFiles,
        int bClusters,
        ExqFunctions<ExqDescriptor<uint64_t,uint64_t,uint64_t>>* functions
    ) {


    this->_functions = functions;//new ExqFunctionsR64(1000,48,16,16,1000,1000);
}