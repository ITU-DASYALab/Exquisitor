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
    template <typename T, typename U, typename V>
    class ExqFunctionsR64 : public ExqFunctions<ExqDescriptor<T,U,V>> {
    public:
        ExqFunctionsR64(int nFeat, int topShift, int idsShift, int ratiosShift, double topDivisor,
                        double ratiosDivisor);
        ExqFunctionsR64(int nFeat, int topShift, int idsShift, int ratiosShift, uint64_t topMask, double topDivisor,
                        uint64_t idsMask, uint64_t ratiosMask, double ratiosDivisor);

        ~ExqFunctionsR64();

        /**
         * @brief convert compressed descriptor representation into actual feature id and value pairs
         * @return
         */
        ExqArray<pair<int,float>> getDescriptorInformation(ExqDescriptor<T,U,V>& descriptor) override;

        /**
         * @brief calculates distance between hyperplane and item
         */
        double distance(vector<double>& model, double bias, ExqDescriptor<T,U,V>& descriptor) override;

        /**
         * @brief rank aggregation using scores from each modality
         */
        void rankItems(vector<ExqItem>& items2Rank, int modalities) override;

    private:
        int nDescFeatures;
        int topFeatureShift;
        int idsFeatureShift;
        int ratiosFeatureShift;
        uint64_t* idsBitShifts;
        uint64_t* ratiosBitShifts;
        uint64_t topMask;
        double topDivisor;
        uint64_t idsMask;
        uint64_t ratiosMask;
        double ratiosDivisor;
    }; //End of class ExqFunctions

    int _compareScoreVis(const void* item1, const void* item2);
    int _compareScoreTxt(const void* item1, const void* item2);
    int _compareScoreAgg(const void* item1, const void* item2);
    void _assignRanking(vector<ExqItem>& items, int mod);
} //End of namespace exq

#endif //EXQUISITOR_EXQFUNCTIONSR64_H

