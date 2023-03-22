//
// Created by Omar Shahbaz Khan on 30/04/2020.
//

#ifndef EXQUISITOR_EXQCLASSIFIER_H
#define EXQUISITOR_EXQCLASSIFIER_H

#include <opencv2/ml.hpp>

namespace exq {

    using std::vector;
    using cv::Ptr;
    using cv::ml::SVMSGD;
    using cv::ml::TrainData;

    class ExqClassifier {
    public:
        ExqClassifier(int totalFeats) {
            _svm = SVMSGD::create();
            resetClassifier();
            // _svm->setSvmsgdType(SVMSGD::ASGD);
            // _svm->setOptimalParameters();
            // _svm->setMarginType(SVMSGD::HARD_MARGIN);
            // _svm->setMarginRegularization(0.0001);
            // _svm->setInitialStepSize(0.0001);
            // _svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 0.01));
            _totalFeats = totalFeats;
        }

        ~ExqClassifier();

        void resetClassifier() {
            if (_svm->isTrained()) {
                _weights.clear();
                _svm->clear();
                _svm.release();
                _svm = Ptr<SVMSGD>();
                _svm = SVMSGD::create();
            }
            _svm->setSvmsgdType(SVMSGD::ASGD);
            _svm->setOptimalParameters();
            _svm->setMarginType(SVMSGD::HARD_MARGIN);
            _svm->setMarginRegularization(0.01);
            _svm->setInitialStepSize(0.01);
            _svm->setStepDecreasingPower(0.75);
            _svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::COUNT, 1000, 1));
        }

        vector<double> train(vector<vector<double>> data, vector<float> labels);

        vector<double> getWeights();

        double getBias();

        inline int getTotalFeats() { return _totalFeats; }

    protected:
        int _totalFeats;
        Ptr<SVMSGD> _svm;
        vector<double> _weights;

    }; //End of class ExqClassifier


} //End of namespace exq

#endif //EXQUISITOR_EXQCLASSIFIER_H
