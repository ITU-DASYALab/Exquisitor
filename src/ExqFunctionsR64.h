#ifndef EXQUISITOR_EXQFUNCTIONSR64_H
#define EXQUISITOR_EXQFUNCTIONSR64_H

#pragma once

#include <string>
#include <vector>

#include "base/ExqDescriptor.h"
#include "base/ExqClassifier.h"
#include "base/ExqItem.h"
#include "base/ExqFunctions.h"

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
        ExqFunctionsR64(int nDescFeat, int iota, int topShift, int idsShift, int ratiosShift, double topDivisor, double ratiosDivisor);
        //Constructor for different compression per modality
        //ExqFunctionsR64(vector<int> nFeat, vector<int> topShift, vector<int> idsShift, vector<int> ratiosShift, vector<double> topDivisor,
        //                vector<double> ratiosDivisor);
        ExqFunctionsR64(int nDescFeat, int iota, int topShift, int idsShift, int ratiosShift, uint64_t topMask, double topDivisor,
                        uint64_t idsMask, uint64_t ratiosMask, double ratiosDivisor);
        //Constructor for different compression per modality
        //ExqFunctionsR64(vector<int> nFeat, vector<int> topShift, vector<int> idsShift, vector<int> ratiosShift, vector<uint64_t> topMask, vector<double> topDivisor,
        //                vector<uint64_t> idsMask, vector<uint64_t> ratiosMask, vector<double> ratiosDivisor);

        ~ExqFunctionsR64();

        int getDescFeatCount() override;
        int getDescriptorSize() override;
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
        void sortItems(vector<ExqItem>& items2Rank, int modality) override;

        /**
         * @brief assign aggregate score for multimodal items
         */
        void assignRanking(vector<ExqItem>& items, int mod) override;

    private:
        int iota;
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

} //End of namespace exq

#endif //EXQUISITOR_EXQFUNCTIONSR64_H

