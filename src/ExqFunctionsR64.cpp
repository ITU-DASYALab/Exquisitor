//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#include "ExqFunctionsR64.h"

using namespace exq;

ExqFunctionsR64::ExqFunctionsR64(int nDescFeat, int topShift, int idsShift, int idsPadding, int ratiosShift, int ratiosPadding) {
    this->nDescFeatures = nDescFeat;
    this->topFeatureShift = topShift;
    this->idsFeatureShift = idsShift;
    this->idsPadding = idsPadding;
    this->ratioFeatureShift = ratiosShift;
    this->ratiosPadding = ratiosPadding;

    this->idsBitShifts = new uint64_t[nFeatures];
    this->ratioBitShifts = new uint64_t[nFeatures];
    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        this->idsBitShifts[i] = i * this->idsFeatureShift + this->idsPadding;
        this->ratioBitShifts[i] = i * this->ratioFeatureShift + this->ratiosPadding;
    }
}

ExqFunctionsR64::~ExqFunctionsR64() {
    delete this->idsBitShifts;
    delete this->ratioBitShifts;
}

ExqArray<pair<int, float> > ExqFunctionsR64::getDescriptorInformation(ExqDescriptor<uint64_t, uint64_t, uint64_t> &descriptor) {
    auto exqArr = new ExqArray<pair<int, float>>(this->nDescFeatures);

    int featId = descriptor.getTop() >> this->topFeatureShift;
    double featVal = (descriptor.getTop() & INIT_MASK) / INIT_N_VALS;
    exqArr->setItem(std::make_pair(featId, featVal), 0);

    for (int i = 0; i < (this->nDescFeatures-1); i++) {
        featId = (descriptor.getFeatureIds() >> this->idsBitShifts[i]) & FEAT_MASK;
        featVal *= ((descriptor.getFeatureRatios() >> this->ratioBitShifts[i]) & FEAT_MASK) / FEAT_DIVISOR;
        exqArr->setItem(std::make_pair(featId, featVal), i+1);
    }

    return *exqArr;
}

double ExqFunctionsR64::distance(ExqClassifier &hyperplane, ExqDescriptor<uint64_t, uint64_t, uint64_t> &descriptor) {
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
