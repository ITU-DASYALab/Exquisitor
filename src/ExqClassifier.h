//
// Created by Omar Shahbaz Khan on 30/04/2020.
//

#ifndef EXQUISITOR_EXQCLASSIFIER_H
#define EXQUISITOR_EXQCLASSIFIER_H

#include "ExqDescriptor.h"

namespace exq {

    using std::vector;

    template <class T>
    class ExqClassifier {
    public:
        ExqClassifier();

        ~ExqClassifier();

        void train(vector<T*> items);



    private:

    }; //End of class ExqClassifier

} //End of namespace exq

#endif //EXQUISITOR_EXQCLASSIFIER_H
