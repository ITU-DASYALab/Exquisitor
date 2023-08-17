#ifndef EXQUISITOR_EXQFUNCTIONSR64_H
#define EXQUISITOR_EXQFUNCTIONSR64_H

#pragma once

#include <string>
#include <vector>

#include "../IExqDescriptor.h"
#include "../ExqClassifier.h"
#include "../ExqItem.h"
#include "../IExqFunctions.h"

namespace exq {

    using std::string;
    using std::vector;

    /**
     * @class ExqFunctions
     * @brief the class contains the scoring and ranking functions utilizing Ratio-i64 compression
     */
    class ExqFunctionsR64 : public IExqFunctions<uint64_t> {
    public:
        ExqFunctionsR64(int nDescFeat, int iota, int topShift, int idsShift, int ratiosShift, double topDivisor, double ratiosDivisor);
        ExqFunctionsR64(int nDescFeat, int iota, int topShift, int idsShift, int ratiosShift, uint64_t topMask, double topDivisor,
                        uint64_t idsMask, uint64_t ratiosMask, double ratiosDivisor);

        ~ExqFunctionsR64();

        int getDescFeatCount() override;
        int getDescriptorSize() override;
        /**
         * @brief convert compressed descriptor representation into actual feature id and value pairs
         * @return
         */
        ExqArray<pair<int,float>> getDescriptorInformation(IExqDescriptor<uint64_t>& descriptor) override;

        /**
         * @brief calculates distance between hyperplane and item
         */
        double distance(vector<float>& model, float bias, IExqDescriptor<uint64_t>& descriptor) override;


        /**
         * @brief rank aggregation using scores from each modality
         */
        void sortItems(vector<ExqItem>& items2Rank, int numMods, vector<double>& modWeights,
                       bool setModRank=false, bool singleMod=false) override;

        /**
         * @brief assign aggregate score for multimodal items
         */
        void assignRanking(vector<ExqItem>& items, int mod, vector<double>& modWeights,
                           bool setModRank) override;

        int getIota() override { return iota; };


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

