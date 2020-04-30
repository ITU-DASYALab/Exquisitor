//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#include "ExqFunctionsR64.h"
#include "IExqFunctions.h"

#include <cmath>

using namespace exq;

ExqFunctionsR64::ExqFunctionsR64(int nDescFeat, int topShift, int idsShift, int ratiosShift, float topDivisor, float ratiosDivisor) {
    this->nDescFeatures = nDescFeat;
    this->topFeatureShift = topShift;
    this->idsFeatureShift = idsShift;
    this->ratiosFeatureShift = ratiosShift;
    this->topDivisor = topDivisor;
    this->ratiosDivisor = ratiosDivisor;

    this->idsBitShifts = new uint64_t[nFeatures];
    this->ratiosBitShifts = new uint64_t[nFeatures];
    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        this->idsBitShifts[i] = i * this->idsFeatureShift + remainder(64, this->idsFeatureShift);
        this->ratiosBitShifts[i] = i * this->ratiosFeatureShift + remainder(64, this->ratiosFeatureShift);
    }
}

ExqFunctionsR64::~ExqFunctionsR64() {
    delete this->idsBitShifts;
    delete this->ratiosBitShifts;
}

inline ExqArray<pair<int, float>> ExqFunctionsR64::getDescriptorInformation(ExqDescriptor<uint64_t,uint64_t,uint64_t> &descriptor) {
    auto exqArr = new ExqArray<pair<int, float>>(this->nDescFeatures);

    int featId = descriptor.getTop() >> this->topFeatureShift;
    double featVal = (descriptor.getTop() & (uint64_t)(pow(2.0, this->topFeatureShift)-1)) / this->topDivisor;
    exqArr->setItem(std::make_pair(featId, featVal), 0);

    uint64_t idsMask = (uint64_t)(pow(2, this->idsFeatureShift)-1);
    uint64_t ratiosMask = (uint64_t)(pow(2, this->ratiosFeatureShift)-1);
    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        featId = (descriptor.getFeatureIds() >> this->idsBitShifts[i]) & idsMask;
        featVal *= ((descriptor.getFeatureRatios() >> this->ratiosBitShifts[i]) & ratiosMask) / this->ratiosDivisor;
        exqArr->setItem(std::make_pair(featId, featVal), i+1);
    }

    return *exqArr;
}

double ExqFunctionsR64::distance(ExqClassifier<uint64_t,uint64_t,uint64_t>& hyperplane, ExqDescriptor<uint64_t,uint64_t,uint64_t> &descriptor) {
    double score = 0.0;
    auto desc = getDescriptorInformation(descriptor);

    //TODO: score = hyperplane->bias;
    for (int i = 0; i < this->nDescFeatures; i++) {
        //TODO: pair<int,float> feature = desc->getItem(i);
        //TODO: score += hyperplane->weightVector[feature.first] * feature.second;
    }

    return score;
}

void ExqFunctionsR64::rankItems(vector<ExqItem> &items2Rank) {

}
