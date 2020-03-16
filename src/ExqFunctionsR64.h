#ifndef EXQUISITOR_EXQFUNCTIONSR64_H
#define EXQUISITOR_EXQFUNCTIONSR64_H

#pragma once

#include <string>
#include <vector>

#include "ExqDescriptor.h"
#include "ExqClassifier.h"
#include "ExqItem.h"
#include "IExqFunctions.h"

namespace exq {

    using std::string;
    using std::vector;

    /**
     * @class ExqFunctions
     * @brief the class contains the scoring and ranking functions utilizing Ratio-i64 compression
     */
    template <typename T, typename U>
    class ExqFunctionsR64 : public IExqFunctions<uint64_t, ExqArray<T>, ExqArray<U>> {
    public:
        ExqFunctionsR64() {

        }

        ~ExqFunctionsR64();

        /**
         * @brief calculates distance between hyperplane and item
         */
        double distance(ExqClassifier& hyperplane, ExqDescriptor<uint64_t, ExqArray<T>, ExqArray<U>>& descriptor) {

        }

        /**
         * @brief rank aggregation using scores from each modality
         */
        void rankItems(vector<ExqItem>& items2Rank) {

        }

    private:
        int nFeatures;
        int topFeatureShift;
        int idsFeatureShift;
        int ratioFeatureShift;
    }; //End of class ExqFunctions

} //End of namespace exq

double      _score_item     (uint32_t id, int mod) {
    int i;
    double score = 0.0;
    uint64_t bitShift[N_TOP_FEAT];
    int featID = 0;
    for(i = 0; i < N_TOP_FEAT; i++) {
        bitShift[i] = i * 16;// + 4;
    }
    score = model->bias[mod];
    double featVal = (featInitScores[mod][id] & INIT_MASK) / INIT_N_VALS;
    int topFeatID = featInitScores[mod][id] >> INIT_SHIFT;
    score += model->w[mod][topFeatID] * featVal;
    for (i = 0; (uint_t) i < N_TOP_FEAT; i++) {
        featVal *= ((featRatios[mod][id] >> bitShift[i]) & FEAT_MASK) / FEAT_DIVISOR;
        featID = (featIds[mod][id] >> bitShift[i]) & FEAT_MASK;
        score += model->w[mod][featID] * featVal;
    }
    //cout << "total_score: " << score << endl;
    return score;
}
#endif //EXQUISITOR_EXQFUNCTIONSR64_H

