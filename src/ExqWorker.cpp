//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqWorker.h"

using namespace exq;

template<typename T>
void ExqWorker::suggest(int& k, vector<ExqItem>& itemsToReturn, vector<double>& model, double bias, int startIndex,
                        int noms, ExqDataHandler<T>& handler, ExqFunctions<T>& functions) {
    //TODO: Utilize DataHandler to get r candidates per modality (r = noms)
    //TODO: Utilize Functions distance function to do this
    //TODO: Utilize Functions rankItems function to fuse modalities and get k candidates
}
