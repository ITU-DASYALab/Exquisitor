//
// Created by Omar Shahbaz Khan on 15/03/2020.
//

#ifndef EXQUISITOR_EXQARRDESCRIPTOR_H
#define EXQUISITOR_EXQARRDESCRIPTOR_H

#pragma once

#include <iostream>
#include <cstdio>

#include "ExqConstants.h"
#include "ExqArray.h"

namespace exq {

    using std::cout;
    using std::endl;

    /**
     * @class Descriptor
     * @brief the class that represents features
     */
    template <typename T,U>
    class ExqArrDescriptor {
    public:
        uint32_t id;
        uint64_t topFeature;
        ExqArray<T> featureIds;
        ExqArray<U> featureRatios;

        ExqArrDescriptor() = default;

        /**
         * @brief constructs the Descriptor
         */
        ExqArrDescriptor(uint32_t id, uint64_t top, ExqArray<T> featIds, ExqArray<U> ratios);

        /**
         * @brief copy a descriptor
         */
        ExqArrDescriptor(ExqArrDescriptor<T,U> *org);

        /**
         * @brief copy a descriptor and set a new id
         */
        ExqArrDescriptor(ExqArrDescriptor<T,U> *org, id_t new_id);

        /**
         * @brief destroys the descriptor object
         */
        ~ExqArrDescriptor() {
            this->id = 0;
            this->topFeature = 0;
            this->featureIds = 0;
            this->featureRatios = 0;
        };

        void printDescriptor();
    }; //End of class ExqDescriptor

    inline ExqArrDescriptor::ExqArrDescriptor(uint32_t id, uint64_t top, ExqArray<T> featIds, ExqArray<U> ratios) {
        this->id = id;
        this->topFeature = top;
        this->featureIds = featIds;
        this->featureRatios = ratios;
    }

    inline ExqArrDescriptor::ExqArrDescriptor(ExqArrDescriptor<T,U> *org) {
        this->id = org->id;
        this->topFeature = org->topFeature;
        this->featureIds = org->featureIds;
        this->featureRatios = org->featureRatios;
    };

    inline ExqArrDescriptor::ExqArrDescriptor(ExqArrDescriptor<T,U> *org, id_t new_id) {
        this->id = new_id;
        this->topFeature = org->topFeature;
        this->featureIds = org->featureIds;
        this->featureRatios = org->featureRatios;
    };

    inline void ExqArrDescriptor::printDescriptor() {
        cout << "Descriptor(id = " << id << ", top = " << topFeature << ", featIds = " << featureIds.getArray() << \
        ", ratios = " << featureRatios.getArray() << endl;
    }

} //End of namespace exq

#endif //EXQUISITOR_EXQARRDESCRIPTOR_H
