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

    //TODO: Create an IExqClassifier that contains relevant virtual functions and rename this to ExqSVMClassifier
    //TODO: Replace ExqClassifier with IExqClassifier in the relevant source files.
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

        ~ExqClassifier() {
            if (_svm->isTrained()) {
                _weights.clear();
                _svm->clear();
                _svm.release();
            }
        }

        vector<float> train(vector<vector<float>> data, vector<float> labels);
        void resetClassifier();
        
        inline vector<float> getWeights() {
            return _weights;
        }

        inline float getBias() {
            return _svm->getShift();
        }

        inline int getTotalFeats() { return _totalFeats; }

    protected:
        int _totalFeats;
        Ptr<SVMSGD> _svm;
        vector<float> _weights;

    }; //End of class ExqClassifier


} //End of namespace exq

#endif //EXQUISITOR_EXQCLASSIFIER_H
