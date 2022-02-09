//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqClassifier.h"

#include <opencv2/ml.hpp>
#include <utility>

using namespace exq;
using std::cout;
using std::endl;

#include <vector>
#include <algorithm>
#include <numeric>

ExqClassifier::ExqClassifier(int totalFeats) {
    _svm = SVMSGD::create();
    _svm->setOptimalParameters();
    //_svm->setMarginType(SVMSGD::SOFT_MARGIN);
    //_svm->setMarginRegularization(0.0001);
    //_svm->setType(SVM::C_SVC);
    //_svm->setKernel(SVM::LINEAR);
    //_svm->setKernel(SVM::POLY);
    //_svm->setGamma(1);
    //_svm->setCoef0(0);
    //_svm->setDegree(1.0);
    //_svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 0.01));
    _trainData = TrainData();
    _totalFeats = totalFeats;
}

ExqClassifier::~ExqClassifier() {
    delete _svm;
}

std::vector<double> ExqClassifier::train(vector<vector<double>> data, vector<float> labels) {
    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[i].size(); j++) {
            if (data[i][j] > 0.0) {
                cout << "Item " << i << " Feature (" << j << "," << data[i][j] << ")" << endl;
            }
        }
        cout << "Label " << labels[i] << endl;
    }
    //Controller calculates scores and creates the 2D data vector
    _trainData.data = std::move(data);
    _trainData.labels = std::move(labels);
    int rows = _trainData.data.size();
    int cols = _trainData.data[0].size();
    cv::Mat labelsMat(rows, 1, CV_32FC1);
    cv::Mat dataMat(rows, cols, CV_32F);
    for (int i = 0; i < rows; i++) {
        //labelsMat.at<float>(i,0) = labels[i];
        labelsMat.at<float>(i,0) = _trainData.labels[i];
        for (int j = 0; j < cols; j++) {
            dataMat.at<float>(i,j) = _trainData.data[i][j];
        }
    }

#ifdef DEBUG_TRAIN_EXTRA
    for (int i = 0; i < rows; i++) {
        cout << "Item " << i << endl;
        for (int j = 0; j < cols; j++) {
            cout << dataMat.at<float>(i,j) << ",";
        }
        cout << endl;
    }
#endif

    cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(dataMat, cv::ml::ROW_SAMPLE, labelsMat);
    _svm->train(td);
    //cv::Mat sv = _svm->getSupportVectors();//SupportVectors();
    cv::Mat sv = _svm->getWeights();//SupportVectors();
    rows = sv.rows;
    cout << "Number of support vectors: " << rows << endl;
    cols = sv.cols;
    cout << "Number of features in support vectors: " << cols << endl;
    //cv::Mat alpha, svidx;
    //_svm->getDecisionFunction(0, alpha, svidx);
    _weights.resize(cols);
    for (int r = 0; r < rows; r++) {
        //cout << "Alpha(" << r << "): " << alpha.at<float>(r) << endl;
        for (int c = 0; c < cols; c++) {
            _weights[c] += sv.at<float>(r,c);
            //_weights[c] += sv.ptr<float>(r)[c];
        }
    }

#ifdef DEBUG_TRAIN
    cout << "(ExqClassifier) Non zero weights: " << endl;
    for (int i = 0; i < _weights.size(); i++) {
        if (_weights[i] != 0.0) {
            cout << i << " " << _weights[i] << endl;
        }
    }
    cout << "bias: " << _svm->getShift() << endl;
#endif

    return _weights;
}

vector<double> ExqClassifier::getWeights() {
    return _weights;
}

double ExqClassifier::getBias() {
    //cv::Mat alpha, svidx;
    return _svm->getShift();//_svm->getDecisionFunction(0, alpha, svidx);
}
