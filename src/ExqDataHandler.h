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
        ~ExqDataHandler() {
            for(auto vecs : *this->items) {
                for (auto desc : *vecs) {
                    delete desc;
                }
                delete vecs;
            }
        }

        vector<vector<ExqDescriptor<T,U,V>*>*>* getItems() {
            return this->items;
        }

    protected:
        //TODO: Change this to DescriptorContainer<T,U,V>
        vector<vector<ExqDescriptor<T,U,V>*>*>* items;
    };

}

#endif //EXQUISITOR_EXQDATAHANDLER_H
