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
        vector<float> labels; //POS/NEG aka. 1.0/-1.0
    };

    class ExqClassifier {
    public:
        ExqClassifier(int totalFeats);

        ~ExqClassifier();

        vector<double> train(vector<vector<double>> data, vector<float> labels);

        vector<double> getWeights();

        double getBias();

        int getTotalFeats();

    protected:
        int _totalFeats;
        //Ptr<SVM> _svm;
        Ptr<SVMSGD> _svm;
        TrainData _trainData;
        vector<double> _weights;

    }; //End of class ExqClassifier

    inline int ExqClassifier::getTotalFeats() { return _totalFeats; }

} //End of namespace exq

#endif //EXQUISITOR_EXQCLASSIFIER_H
