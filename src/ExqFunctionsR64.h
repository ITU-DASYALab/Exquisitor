#ifndef EXQUISITOR_EXQFUNCTIONSR64_H
#define EXQUISITOR_EXQFUNCTIONSR64_H

#pragma once

#include <string>
#include <vector>

#include "ExqDescriptor.h"
#include "ExqClassifier.h"
#include "ExqItem.h"
#include "ExqFunctions.h"

namespace exq {

    using std::string;
    using std::vector;

    /**
     * @class ExqFunctions
     * @brief the class contains the scoring and ranking functions utilizing Ratio-i64 compression
     */
    class ExqFunctionsR64 : public ExqFunctions {
    public:
        ExqFunctionsR64(int nFeat, int topShift, int idsShift, int ratiosShift, float topDivisor, float ratiosDivisor);

        ~ExqFunctionsR64();

        /**
         * @brief convert compressed descriptor representation into actual feature id and value pairs
         * @return
         */
        ExqArray<pair<int,float>> getDescriptorInformation(ExqDescriptor<uint64_t,uint64_t,uint64_t>& descriptor);

        /**
         * @brief calculates distance between hyperplane and item
         */
        double distance(ExqClassifier<uint64_t,uint64_t,uint64_t>& hyperplane, ExqDescriptor<uint64_t,uint64_t,uint64_t>& descriptor);

        /**
         * @brief rank aggregation using scores from each modality
         */
        void rankItems(vector<ExqItem>& items2Rank);

    private:
        int nDescFeatures;
        int topFeatureShift;
        int idsFeatureShift;
        int ratiosFeatureShift;
        uint64_t* idsBitShifts;
        uint64_t* ratiosBitShifts;
        float topDivisor;
        float ratiosDivisor;
    }; //End of class ExqFunctions

} //End of namespace exq

#endif //EXQUISITOR_EXQFUNCTIONSR64_H

