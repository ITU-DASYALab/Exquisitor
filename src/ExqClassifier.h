//
// Created by Omar Shahbaz Khan on 30/04/2020.
//

#ifndef EXQUISITOR_EXQCLASSIFIER_H
#define EXQUISITOR_EXQCLASSIFIER_H

#include <opencv2/ml.hpp>
#include "ExqDescriptor.h"

namespace exq {

    using std::vector;
    using cv::Ptr;
    using cv::ml::SVMSGD;
    using cv::ml::SVM;

    //struct TrainData {
    //    double*** x; //VIS/TXT->POS/NEG->FEATURE
    //    int* i;      //POS/NEG: Current Index
    //    int* nEx;    //POS/NEG: Total number of examples
    //};

    struct TrainData {
        vector<vector<double>> data; //Item->Feature
        vector<double> labels; //POS/NEG aka. 1.0/-1.0
    };

    class ExqClassifier {
    public:
        ExqClassifier();

        ~ExqClassifier();

        std::vector<double> trainSVM(vector<vector<double>> data, vector<double> labels);

    protected:
        //Ptr<SVMSGD> svmsgd;
        Ptr<SVM> svm;
        TrainData trainData;
    }; //End of class ExqClassifier

} //End of namespace exq

#endif //EXQUISITOR_EXQCLASSIFIER_H
