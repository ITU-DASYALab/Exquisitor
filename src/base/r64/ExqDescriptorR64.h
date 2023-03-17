//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#ifndef EXQUISITOR_EXQDESCRIPTORR64_H
#define EXQUISITOR_EXQDESCRIPTORR64_H

#pragma once

#include <iostream>
#include <cstdio>
#include <tuple>
#include <array>
#include <cstdint>
#include <cinttypes>

#include "../ExqArray.h"
#include "../IExqDescriptor.h"

namespace exq {

    /**
     * @class ExqDescriptor
     * @brief the class that represents features
     */
    class ExqDescriptorR64 : public IExqDescriptor<uint64_t> {
    public:
        uint64_t topFeature;
        ExqArray<uint64_t> featureIds;
        ExqArray<uint64_t> featureRatios;
        ExqDescriptorR64() = default;

        /**
         * @brief constructs the Descriptor
         */
        ExqDescriptorR64(uint32_t itemId, uint64_t top, ExqArray<uint64_t> featIds, ExqArray<uint64_t> ratios)
        : IExqDescriptor<uint64_t>() {
            _id = itemId;
            topFeature = top;
            featureIds = featIds;
            featureRatios = ratios;
        }

        /**
         * @brief copy a descriptor
         */
        ExqDescriptorR64(ExqDescriptorR64 *org) : IExqDescriptor<uint64_t>() {
            _id = org->_id;
            topFeature = org->topFeature;
            featureIds = org->featureIds;
            featureRatios = org->featureRatios;
        }

        /**
         * @brief copy a descriptor and set a new id
         */
        ExqDescriptorR64(ExqDescriptorR64 *org, uint32_t new_id) : IExqDescriptor<uint64_t>() {
            _id = new_id;
            topFeature = org->topFeature;
            featureIds = org->featureIds;
            featureRatios = org->featureRatios;
        }

        ExqDescriptorR64(FILE* file, int iota = 1, bool gobbleDist = 0) : IExqDescriptor<uint64_t>() {
            size_t res;
            res = fread(&this->_id, sizeof(int), 1, file);
            if (gobbleDist) {
                res = fread(&this->topFeature, sizeof(int), 1, file);
            }
            featureIds = ExqArray<uint64_t>(iota);
            featureRatios = ExqArray<uint64_t>(iota);
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
        ~ExqDescriptorR64() {}

        inline void setItemId(uint32_t itemId) { _id = itemId; }

        // void setTop(uint64_t top) { topFeature = top; }

        // void setFeatureIds(uint64_t ids) { featureIds = ids; }

        // void setFeatureRatios(uint64_t ratios) { featureRatios = ratios; }

        inline uint32_t getId() { return _id; }

        uint64_t getTop() override { return this->topFeature; }

        ExqArray<uint64_t>* getFeatureIds() override { return &this->featureIds; }

        ExqArray<uint64_t>* getFeatureRatios() { return &this->featureRatios; }

    }; //End of class ExqDescriptor

} //End of namespace exq


#endif //EXQUISITOR_IEXQDESCRIPTOR_H
