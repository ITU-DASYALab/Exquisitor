//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQR64DESCRIPTOR_H
#define EXQUISITOR_EXQR64DESCRIPTOR_H

#pragma once

#include <iostream>
#include <cstdio>

#include "ExqConstants.h"

namespace exq {

    using std::cout;
    using std::endl;

/**
 * @class Descriptor
 * @brief the class that represents features
 */
    class ExqR64Descriptor {
    public:
        uint32_t id;
        uint64_t topFeature;
        uint64_t featureIds;
        uint64_t featureRatios;

        ExqR64Descriptor() = default;

        /**
         * @brief constructs the Descriptor
         */
        ExqR64Descriptor(uint32_t id, uint64_t top, uint64_t featIds, uint64_t ratios);

        /**
         * @brief copy a descriptor
         */
        ExqR64Descriptor(ExqR64Descriptor *org);

        /**
         * @brief copy a descriptor and set a new id
         */
        ExqR64Descriptor(ExqR64Descriptor *org, id_t new_id);

        /**
         * @brief destroys the descriptor object
         */
        ~ExqR64Descriptor() {
            this->id = 0;
            this->topFeature = 0;
            this->featureIds = 0;
            this->featureRatios = 0;
        };

        void printDescriptor();
    }; //End of class ExqDescriptor

    inline ExqR64Descriptor::ExqR64Descriptor(uint32_t id, uint64_t top, uint64_t featIds, uint64_t ratios) {
        this->id = id;
        this->topFeature = top;
        this->featureIds = featIds;
        this->featureRatios = ratios;
    }

    inline ExqR64Descriptor::ExqR64Descriptor(ExqR64Descriptor *org) {
        this->id = org->id;
        this->topFeature = org->topFeature;
        this->featureIds = org->featureIds;
        this->featureRatios = org->featureRatios;
    };

    inline ExqR64Descriptor::ExqR64Descriptor(ExqR64Descriptor *org, id_t new_id) {
        this->id = new_id;
        this->topFeature = org->topFeature;
        this->featureIds = org->featureIds;
        this->featureRatios = org->featureRatios;
    };

    inline void ExqR64Descriptor::printDescriptor() {
        cout << "R64 Descriptor(id = " << id << ", top = " << topFeature << ", featIds = " << featureIds << ", ratios = " \
        << featureRatios<< endl;
    }

} //End of namespace exq


#endif //EXQUISITOR_EXQR64DESCRIPTOR_H
