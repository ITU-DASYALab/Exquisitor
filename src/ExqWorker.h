//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQWORKER_H
#define EXQUISITOR_EXQWORKER_H

#pragma once

#include <vector>

#include "ExqDescriptor.h"
#include "ExqDataHandler.h"
#include "ExqFunctions.h"
#include "ExqItem.h"

namespace exq {

    using std::vector;

    class ExqWorker {
    public:
        template<typename T>
        void suggest(int& k, vector<ExqItem>& itemsToReturn, vector<double>& model, double bias, int currentSegment,
                     int totalSegments, int noms, int modalities, ExqDataHandler<T>& handler,
                     ExqFunctions<T>& functions);
    private:

    }; //End of class ExqWorker

} //End of namespace exq

#endif //EXQUISITOR_EXQWORKER_H
