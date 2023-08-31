//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqClassifier.h"

#include <opencv2/ml.hpp>

using namespace exq;
using std::cout;
using std::endl;
using std::vector;

using cv::Ptr;
using cv::ml::TrainData;
using cv::ml::SVMSGD;

vector<float> ExqClassifier::train(vector<vector<float>> data, vector<float> labels) {
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    for (int i = 0; i < (int)data.size(); i++) {
        for (int j = 0; j < (int)data[i].size(); j++) {
            if (data[i][j] > 0.0) {
                cout << "Item " << i << " Feature (" << j << "," << data[i][j] << ")" << endl;
            }
        }
        cout << "Label " << labels[i] << endl;
    }
#endif
    //Controller calculates scores and creates the 2D data vector
    int rows = (int) data.size();
    int cols = (int) data[0].size();
    cv::Mat labelsMat(rows, 1, CV_32FC1);
    cv::Mat dataMat(rows, cols, CV_32F);
    for (int i = 0; i < rows; i++) {
        labelsMat.at<float>(i,0) = labels[i];
        for (int j = 0; j < cols; j++) {
            dataMat.at<float>(i,j) = data[i][j];
        }
    }

#if defined(DEBUG_EXTRA) || defined(DEBUG_TRAIN_EXTRA)
    for (int i = 0; i < rows; i++) {
        cout << "Item " << i << endl;
        for (int j = 0; j < cols; j++) {
            cout << dataMat.at<float>(i,j) << ",";
        }
        cout << endl;
    }
#endif

    _svm->train(dataMat, cv::ml::ROW_SAMPLE, labelsMat);
    cv::Mat sv = _svm->getWeights();
    rows = sv.rows;
    cols = sv.cols;
#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "Number of support vectors: " << rows << endl;
    cout << "Number of features in support vectors: " << cols << endl;
#endif
    _weights.resize(cols);
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            _weights[c] += sv.at<float>(r,c);
        }
    }

#if defined(DEBUG) || defined(DEBUG_TRAIN)
    cout << "(ExqClassifier) Non zero weights: " << endl;
    for (int i = 0; i < (int)_weights.size(); i++) {
        if (_weights[i] != 0.0) {
            cout << i << " " << _weights[i] << endl;
        }
    }
    cout << "bias: " << _svm->getShift() << endl;
#endif
    return _weights;
}

void ExqClassifier::resetClassifier() {
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
    _svm->setMarginRegularization(0.01f);
    _svm->setInitialStepSize(0.01f);
    _svm->setStepDecreasingPower(0.75f);
    _svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::COUNT, 1000, 1));
}
