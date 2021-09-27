//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#include "ExqFunctionsR64.h"

#include <cmath>

using namespace exq;


template <typename T, typename U, typename V>
ExqFunctionsR64<T,U,V>::ExqFunctionsR64(int nDescFeat, int iota, int topShift, int idsShift, int ratiosShift, double topDivisor,
                                        double ratiosDivisor) {
    this->nDescFeatures = nDescFeat;
    this->topFeatureShift = topShift;
    this->idsFeatureShift = idsShift;
    this->ratiosFeatureShift = ratiosShift;
    this->topMask = (uint64_t)(pow(2.0, this->topFeatureShift)-1);
    this->topDivisor = topDivisor;
    this->idsMask = (uint64_t)(pow(2, this->idsFeatureShift)-1);
    this->ratiosMask = (uint64_t)(pow(2, this->ratiosFeatureShift)-1);
    this->ratiosDivisor = ratiosDivisor;

    //TODO: Make use of iota to get correct values
    this->idsBitShifts = new uint64_t[this->nDescFeatures-1];
    this->ratiosBitShifts = new uint64_t[this->nDescFeatures-1];
    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        this->idsBitShifts[i] = i * this->idsFeatureShift + remainder(64, this->idsFeatureShift);
        this->ratiosBitShifts[i] = i * this->ratiosFeatureShift + remainder(64, this->ratiosFeatureShift);
    }
}

template <typename T, typename U, typename V>
ExqFunctionsR64<T,U,V>::ExqFunctionsR64(int nDescFeat, int iota, int topShift, int idsShift, int ratiosShift, uint64_t topMask,
                                        double topDivisor, uint64_t idsMask, uint64_t ratiosMask, double ratiosDivisor) {
    this->nDescFeatures = nDescFeat;
    this->topFeatureShift = topShift;
    this->idsFeatureShift = idsShift;
    this->ratiosFeatureShift = ratiosShift;
    this->topMask = topMask;
    this->topDivisor = topDivisor;
    this->idsMask = idsMask;
    this->ratiosMask = ratiosMask;
    this->ratiosDivisor = ratiosDivisor;

    this->idsBitShifts = new uint64_t[this->nDescFeatures-1];
    this->ratiosBitShifts = new uint64_t[this->nDescFeatures-1];
    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        this->idsBitShifts[i] = i * this->idsFeatureShift + remainder(64, this->idsFeatureShift);
        this->ratiosBitShifts[i] = i * this->ratiosFeatureShift + remainder(64, this->ratiosFeatureShift);
    }
}

template <typename T, typename U, typename V>
ExqFunctionsR64<T,U,V>::~ExqFunctionsR64() {
    delete this->idsBitShifts;
    delete this->ratiosBitShifts;
}

template <typename T, typename U, typename V>
int ExqFunctionsR64<T, U, V>::getDescFeatCount() {
    return nDescFeatures;
}

template <typename T, typename U, typename V>
/// Decompress item and return the results in an ExqArray
inline ExqArray<pair<int, float>> ExqFunctionsR64<T,U,V>::getDescriptorInformation(ExqDescriptor<T,U,V> &descriptor) {
    auto exqArr = new ExqArray<pair<int, float>>(this->nDescFeatures);

    int featId = descriptor.getTop() >> this->topFeatureShift;
    double featVal = (descriptor.getTop() & this->topMask) / this->topDivisor;
    exqArr->setItem(std::make_pair(featId, featVal), 0);

    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        featId = (descriptor.getFeatureIds() >> this->idsBitShifts[i]) & this->idsMask;
        featVal *= ((descriptor.getFeatureRatios() >> this->ratiosBitShifts[i]) & this->ratiosMask)/this->ratiosDivisor;
        exqArr->setItem(std::make_pair(featId, featVal), i+1);
    }

    return *exqArr;
}

///
/// Calculate item score and return distance from Hyperplane
/// \tparam T - Compressed Integer Type for Top Feature Id and Value
/// \tparam U - Compressed Integer Type for Top 2..t Feature Ids
/// \tparam V - Compressed Integer Type for Top 2..t Feature Values
/// \param model - Hyperplane weights
/// \param bias - Bias from SVM
/// \param descriptor - Descriptor containing all feature information
/// \return
template <typename T, typename U, typename V>
inline double ExqFunctionsR64<T,U,V>::distance(vector<double>& model, double bias, ExqDescriptor<T,U,V> &descriptor) {
    double score = bias;
    int featId = descriptor.getTop() >> this->topFeatureShift;
    double featVal = (descriptor.getTop() & this->topMask) / this->topDivisor;

    score += model[featId] * featVal;
    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        featId = (descriptor.getFeatureIds() >> this->idsBitShifts[i]) & this->idsMask;
        featVal *= ((descriptor.getFeatureRatios() >> this->ratiosBitShifts[i]) & this->ratiosMask)/this->ratiosDivisor;
        score += model[featId] * featVal;
    }

    return score;
}

template <typename T, typename U, typename V>
void ExqFunctionsR64<T,U,V>::sortItems(vector<ExqItem> &items2Rank, int modality) {
    double rank = 0.0;

    //TODO: For more flexibility/adaptability for modalities implement own quicksort function
    if (modality == 0) {
        std::sort(items2Rank.begin(), items2Rank.end(), [](const ExqItem& lhs, const ExqItem& rhs) {
            return lhs.distance[VIS] < rhs.distance[VIS];
        });
    } else if (modality == 1) {
        std::sort(items2Rank.begin(), items2Rank.end(), [](const ExqItem& lhs, const ExqItem& rhs) {
            return lhs.distance[TXT] < rhs.distance[TXT];
        });
    } else {
        std::sort(items2Rank.begin(), items2Rank.end(), [](const ExqItem& lhs, const ExqItem& rhs) {
            return lhs.distance[VIS] < rhs.distance[VIS];
        });
        assignRanking(items2Rank, VIS);

        std::sort(items2Rank.begin(), items2Rank.end(), [](const ExqItem& lhs, const ExqItem& rhs) {
            return lhs.distance[TXT] < rhs.distance[TXT];
        });
        assignRanking(items2Rank, TXT);

        std::sort(items2Rank.begin(), items2Rank.end(), [](const ExqItem& lhs, const ExqItem& rhs) {
            return lhs.aggScore > rhs.aggScore;
        });
    }
}

template <typename T, typename U, typename V>
void ExqFunctionsR64<T,U,V>::assignRanking(vector<ExqItem>& items, int mod) {
    double rank = 0.0;
    items[0].aggScore += 0.0;

    for (int i = 1; i < items.size(); i++) {
        if (items[i].distance[mod] == items[i-1].distance[mod]) {
            items[i].aggScore += rank;
        } else {
            items[i].aggScore += i;
            rank = i;
        }
    }
}

template<typename T, typename U, typename V>
int ExqFunctionsR64<T, U, V>::getDescriptorSize() {
    return sizeof(T) + iota * sizeof(U) * 2;
}

template class exq::ExqFunctionsR64<uint64_t,uint64_t,uint64_t>;
