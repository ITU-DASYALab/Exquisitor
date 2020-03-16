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

#include "ExqArray.h"

namespace exq {

    /**
     * @class ExqDescriptor
     * @brief the class that represents features
     */
    template <typename T, typename U, typename V>
    class ExqDescriptor {
    public:
        ExqDescriptor() = default;

        /**
         * @brief constructs the Descriptor
         */
        ExqDescriptor(uint32_t itemId, uint64_t top, uint64_t featIds, uint64_t ratios) {
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
        ExqDescriptor(ExqDescriptor *org, id_t new_id) {
            id = new_id;
            topFeature = org->topFeature;
            featureIds = org->featureIds;
            featureRatios = org->featureRatios;
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

        U getFeatureIds() { return this->featureIds; }

        V getFeatureRatios() { return this->featureRatios; }

    protected:
        uint32_t id;
        T topFeature;
        U featureIds;
        V featureRatios;
    }; //End of class ExqDescriptor

} //End of namespace exq


#endif //EXQUISITOR_IEXQDESCRIPTOR_H
