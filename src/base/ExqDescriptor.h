//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#ifndef EXQUISITOR_EXQDESCRIPTOR_H
#define EXQUISITOR_EXQDESCRIPTOR_H

#pragma once

#include <iostream>
#include <cstdio>
#include <tuple>
#include <array>
#include <cstdint>
#include <cinttypes>

#include "ExqArray.h"

namespace exq {

    /**
     * @class ExqDescriptor
     * @brief the class that represents features
     */
    template <typename T, typename U, typename V>
    class ExqDescriptor {
    public:
        uint32_t id;
        T topFeature;
        ExqArray<U> featureIds;
        ExqArray<V> featureRatios;
        ExqDescriptor() = default;

        /**
         * @brief constructs the Descriptor
         */
        ExqDescriptor(uint32_t itemId, T top, ExqArray<U> featIds, ExqArray<V> ratios) {
            id = itemId;
            topFeature = top;
            featureIds = featIds;
            featureRatios = ratios;
        }

        /**
         * @brief copy a descriptor
         */
        ExqDescriptor(ExqDescriptor *org) {
            id = org->id;
            topFeature = org->topFeature;
            featureIds = org->featureIds;
            featureRatios = org->featureRatios;
        }

        /**
         * @brief copy a descriptor and set a new id
         */
        ExqDescriptor(ExqDescriptor *org, uint32_t new_id) {
            id = new_id;
            topFeature = org->topFeature;
            featureIds = org->featureIds;
            featureRatios = org->featureRatios;
        }

        ExqDescriptor(FILE* file, int iota = 1, bool gobbleDist = 0) {
            size_t res;
            res = fread(&this->id, sizeof(int), 1, file);
            if (gobbleDist) {
                res = fread(&this->topFeature, sizeof(int), 1, file);
            }
            featureIds = ExqArray<U>(iota);
            featureRatios = ExqArray<V>(iota);
            res = fread(&this->topFeature, sizeof(uint64_t), 1, file);
            uint64_t featId;
            uint64_t ratios;
            for (int i = 0; i < iota; i++) {
                res = fread(&featId, sizeof(uint64_t), 1, file);
                featureIds.setItem(featId, i);
                res = fread(&ratios, sizeof(uint64_t), 1, file);
                featureRatios.setItem(ratios, i);
            }
            if (res == 0) std::cout << "Error reading descriptor" << std::endl;
        }

        /**
         * @brief destroys the descriptor object
         */
        ~ExqDescriptor() {}

        void setItemId(uint32_t itemId) { id = itemId; }

        void setTop(T top) { topFeature = top; }

        void setFeatureIds(U ids) { featureIds = ids; }

        void setFeatureRatios(V ratios) { featureRatios = ratios; }

        uint32_t getId() { return id; }

        T getTop() { return this->topFeature; }

        U getFeatureIds(int i) { return this->featureIds.getItem(i); }

        V getFeatureRatios(int i) { return this->featureRatios.getItem(i); }

    }; //End of class ExqDescriptor

} //End of namespace exq


#endif //EXQUISITOR_IEXQDESCRIPTOR_H
