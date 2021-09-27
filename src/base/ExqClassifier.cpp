//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqClassifier.h"

#include <opencv2/ml.hpp>

using namespace exq;
using std::cout;
using std::endl;

ExqClassifier::ExqClassifier(int totalFeats) {
    _svm = SVM::create();
    _svm->setType(SVM::C_SVC);
    _svm->setKernel(SVM::POLY);
    _svm->setGamma(1);
    _svm->setCoef0(0);
    _svm->setDegree(1.0);
    _trainData = TrainData();
    _totalFeats = totalFeats;
}

ExqClassifier::~ExqClassifier() {
    delete _svm;
}

std::vector<double> ExqClassifier::train(vector<vector<double>> data, vector<short> labels) {
    //Controller calculates scores and creates the 2D data vector
    _trainData.data = data;
    _trainData.labels = labels;
    int rows = _trainData.data.size();
    int cols = _trainData.data[0].size();
    //cv::Mat labelsMat(rows, 1, CV_32F);
    cv::Mat labelsMat(rows, 1, CV_32S);
    cv::Mat dataMat(rows, cols, CV_32F);
    for (int i = 0; i < rows; i++) {
        //labelsMat.at<float>(i,0) = labels[i];
        labelsMat.at<short>(i,0) = _trainData.labels[i];
        for (int j = 0; j < cols; j++) {
            dataMat.at<float>(i,j) = _trainData.data[i][j];
        }
    }

    _svm->train(dataMat, cv::ml::ROW_SAMPLE, labelsMat);
    cv::Mat weights = _svm->getSupportVectors();
    rows = weights.rows;
    cols = weights.cols;
    _weights.resize(cols);
    if (weights.isContinuous()) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                _weights[c] += weights.at<float>(r,c);
            }
        }
    }

    return _weights;
}

vector<double> ExqClassifier::getWeights() {
    return _weights;
}

double ExqClassifier::getBias() {
    cv::Mat alpha, svidx;
    return _svm->getDecisionFunction(0, alpha, svidx);
}
