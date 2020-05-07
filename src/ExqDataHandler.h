//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#ifndef EXQUISITOR_EXQDATAHANDLER_H
#define EXQUISITOR_EXQDATAHANDLER_H

#include <vector>

#include "ExqDescriptor.h"

namespace exq {

    using std::vector;

    template <typename T, typename U, typename V>
    class ExqDataHandler {
    public:
        /**
         * Returns descriptor with for item i. Assumes modality is 0.
         * @param i - item id
         * @return Descriptor for item i.
         */
        ExqDescriptor<T,U,V>* getItem(uint32_t i) {
            return this->_descriptors[0][i];
        }

        /**
         * Returns item with id i from specific modality
         * @param i - item id
         * @param mod - modality
         * @return Descriptor for item i in modality mod
         */
        ExqDescriptor<T,U,V>* getItem(uint32_t i, int mod) {
           return this->_descriptors[mod][i];
        }

        int getTotalItemsCount(int mod) {
            return this->_descriptors[mod].size();
        };

    protected:
        vector<vector<ExqDescriptor<T,U,V>*>> _descriptors;
    };

}

#endif //EXQUISITOR_EXQDATAHANDLER_H
