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
    using cv::ml::TrainData;

    class ExqClassifier {
    public:
        ExqClassifier(int totalFeats);

        ~ExqClassifier();

        void resetClassifier();

        vector<double> train(vector<vector<double>> data, vector<float> labels);

        vector<double> getWeights();

        double getBias();

        int getTotalFeats();

    protected:
        int _totalFeats;
        Ptr<SVMSGD> _svm;
        Ptr<cv::ml::TrainData> _trainData;
        vector<double> _weights;

    }; //End of class ExqClassifier

    inline int ExqClassifier::getTotalFeats() { return _totalFeats; }

} //End of namespace exq

#endif //EXQUISITOR_EXQCLASSIFIER_H
