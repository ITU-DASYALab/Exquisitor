//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#include "ExqFunctionsR64.h"

#include <cmath>

using namespace exq;
using std::cout;
using std::endl;


ExqFunctionsR64::ExqFunctionsR64(int nDescFeat, int iota, int topShift, int idsShift, int ratiosShift, uint64_t topMask,
                                 float topDivisor, uint64_t idsMask, uint64_t ratiosMask, float ratiosDivisor) {
    this->iota = iota;
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
        this->idsBitShifts[i] = i * this->idsFeatureShift + (uint64_t) remainder(64, this->idsFeatureShift);
        this->ratiosBitShifts[i] = i * this->ratiosFeatureShift + (uint64_t) remainder(64, this->ratiosFeatureShift);
    }
}

ExqFunctionsR64::ExqFunctionsR64(int nDescFeat, int iota, int topShift,  int idsShift, int ratiosShift, int topMask, float topDivisor,
                                 int idsMask, int ratiosMask, float ratiosDivisor) {
    this->iota = iota;
    this->nDescFeatures = nDescFeat;
    this->topFeatureShift = topShift;
    this->idsFeatureShift = idsShift;
    this->ratiosFeatureShift = ratiosShift;
    this->topMask = (uint64_t)(pow(2.0, topMask)-1);
    cout << "(ExqFunc) topMask: " << this->topMask << endl;
    this->topDivisor = topDivisor;
    this->idsMask = (uint64_t)(pow(2, idsMask)-1);
    cout << "(ExqFunc) idsMask: " << this->idsMask << endl;
    this->ratiosMask = (uint64_t)(pow(2, ratiosMask)-1);
    cout << "(ExqFunc) ratiosMask: " << this->ratiosMask << endl;
    this->ratiosDivisor = ratiosDivisor;

    this->idsBitShifts = new uint64_t[this->nDescFeatures-1];
    this->ratiosBitShifts = new uint64_t[this->nDescFeatures-1];
    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        this->idsBitShifts[i] = i * this->idsFeatureShift + (uint64_t) remainder(64, this->idsFeatureShift);
        this->ratiosBitShifts[i] = i * this->ratiosFeatureShift + (uint64_t) remainder(64, this->ratiosFeatureShift);
    }
}



ExqFunctionsR64::~ExqFunctionsR64() {
    delete this->idsBitShifts;
    delete this->ratiosBitShifts;
}

int ExqFunctionsR64::getDescFeatCount() {
    return nDescFeatures;
}

/// Decompress item and return the results in an ExqArray
inline ExqArray<pair<int, float>> ExqFunctionsR64::getDescriptorInformation(IExqDescriptor<uint64_t> &descriptor) {
    uint64_t top = *(uint64_t*) descriptor.getTop();
    auto featureIds = (ExqArray<uint64_t>*) descriptor.getFeatureIds();
    auto featureRatios = (ExqArray<uint64_t>*) descriptor.getFeatureRatios();

#ifdef DEBUG
    cout << "(ExqFncR64) Getting descriptor information for descriptor " << descriptor.id << endl;
    cout << "(ExqFncR64) Top: " << descriptor.getTop() << " Ids: " << descriptor.getFeatureIds() << " Ratio: "
    << descriptor.getFeatureRatios() << endl;
#endif
    auto exqArr = ExqArray<pair<int, float>>(this->nDescFeatures);

    int featId = (int) (top >> this->topFeatureShift);
#ifdef DEBUG
    cout << "(ExqFncR64) Top Feature ID: " << featId << endl;
#endif
    float featVal = (top & this->topMask) / this->topDivisor;
#ifdef DEBUG
    cout << "(ExqFncR64) Top Feature Value: " << featVal << endl;
#endif
    exqArr.setItem(std::make_pair(featId, featVal), 0);

    for (int i = 0; i < iota; i++) {
        for (int j = 0; j < (this->nDescFeatures-1); j++) {
            featId = (int) ((featureIds->getItem(i) >> this->idsBitShifts[j]) & this->idsMask);
#ifdef DEBUG
            cout << "(ExqFncR64) Feature ID: " << featId << endl;
#endif
            featVal *= ((featureRatios->getItem(i) >> this->ratiosBitShifts[j]) & \
                       this->ratiosMask)/this->ratiosDivisor;
#ifdef DEBUG
            cout << "(ExqFncR64) Feature Val: " << featVal << endl;
#endif
            exqArr.setItem(std::make_pair(featId, featVal), j+1);
        }
    }

    return exqArr;
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
inline float ExqFunctionsR64::distance(vector<float>& model, float bias, IExqDescriptor<uint64_t> &descriptor) {
    uint64_t top = *(uint64_t*) descriptor.getTop();
    auto featureIds = (ExqArray<uint64_t>*) descriptor.getFeatureIds();
    auto featureRatios = (ExqArray<uint64_t>*) descriptor.getFeatureRatios();

    float score = bias;
    int featId = (int) (top >> this->topFeatureShift);
    float featVal = (top & this->topMask) / this->topDivisor;
    score += model[featId] * featVal;
    for (int i = 0; i < iota; i++) {
        for (int j = 0; j < (this->nDescFeatures - 1); j++) {
            featId = (int) ((featureIds->getItem(i) >> this->idsBitShifts[j]) & this->idsMask);
            featVal *= ((featureRatios->getItem(i) >> this->ratiosBitShifts[j]) & this->ratiosMask) /
                       this->ratiosDivisor;
            score += model[featId] * featVal;
        }
    }

#if defined(DEBUG_EXTRA) || defined(DEBUG_TRAIN_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
    cout << "Descriptor " << descriptor.id << ": " << score << endl;
#endif
    return score;
}

void ExqFunctionsR64::sortItems(vector<ExqItem> &items2Rank, int numMods, vector<float>& modWeights,
                                bool setModRank, bool singleMod) {
    if (numMods > 1 && !singleMod) {
        for (int m = 0; m < numMods; m++) {
            std::sort(items2Rank.begin(), items2Rank.end(), [m](const ExqItem& lhs, const ExqItem& rhs) {
                return lhs.distance[m] > rhs.distance[m];
            });
            assignRanking(items2Rank, m, modWeights, setModRank);
        }
        std::sort(items2Rank.begin(), items2Rank.end(), [](const ExqItem& lhs, const ExqItem& rhs) {
            return lhs.aggScore < rhs.aggScore;
        });
    } else {
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        cout << "(ExqFunc) Single modality ranking" << endl;
        for (int i = 0; i < (int)items2Rank.size(); i++) {
            cout << "(" << i << "," << items2Rank[i].itemId << "," << items2Rank[i].distance[0] << "), ";
        }
        cout << endl;
#endif
        int mod = singleMod;
        std::sort(items2Rank.begin(), items2Rank.end(), [mod](const ExqItem& lhs, const ExqItem& rhs) {
            return lhs.distance[mod] > rhs.distance[mod];
        });
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
        for (int i = 0; i < (int)items2Rank.size(); i++) {
            cout << "(" << i << "," << items2Rank[i].itemId << "," << items2Rank[i].distance[0] << "), ";
        }
        cout << endl;
#endif
    }
}

void ExqFunctionsR64::assignRanking(vector<ExqItem>& items, int mod, vector<float>& modWeights,
                                    bool setModRank) {
    float rank = 0.0;
    items[0].aggScore += 0.0;
    items[0].modRank.push_back(0.0);
    float score_position = 0.0;

    if (setModRank) {
        float position = 0.0;
        for (int i = 1; i < (int) items.size(); i++) {
            if (items[i].distance[mod] == items[i - 1].distance[mod]) {
                items[i].aggScore += rank;
            } else {
                score_position++;
                items[i].aggScore += score_position * modWeights[mod];
                rank = score_position * modWeights[mod];
                position = (float)items.size()-score_position;
            }
            items[i].modRank.push_back(position);
            //items[i].modRank.push_back((((double)items.size()-position)/(double)items.size()));
        }
    } else {
        for (int i = 1; i < (int) items.size(); i++) {
            if (items[i].distance[mod] == items[i - 1].distance[mod]) {
                items[i].aggScore += rank;
            } else {
                score_position++;
                items[i].aggScore += score_position * modWeights[mod];
                rank = score_position * modWeights[mod];
            }
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
            cout << "(ExqFunc) Item " << items[i].itemId << " aggScore: " << items[i].aggScore << endl;
#endif
        }
    }
}

int ExqFunctionsR64::getDescriptorSize() {
    return sizeof(uint64_t) + (iota * sizeof(uint64_t)) + (iota * sizeof(uint64_t));
}