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

    struct TrainData {
        vector<vector<double>> data; //Item->Feature
        vector<short> labels; //POS/NEG aka. 1.0/-1.0
    };

    class ExqClassifier {
    public:
        ExqClassifier();

        ~ExqClassifier();

        vector<double> trainSVM(vector<vector<double>> data, vector<short> labels);

        vector<double> getWeights();

    protected:
        Ptr<SVM> svm;
        TrainData trainData;
        vector<double> weights;

    }; //End of class ExqClassifier


} //End of namespace exq

#endif //EXQUISITOR_EXQCLASSIFIER_H
