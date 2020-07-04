//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqClassifier.h"

#include <opencv2/ml.hpp>

using namespace exq;
using std::cout;
using std::endl;

ExqClassifier::ExqClassifier() {
    this->svm = SVM::create();
    this->svm->setType(SVM::C_SVC);
    this->svm->setKernel(SVM::POLY);
    this->svm->setGamma(1);
    this->svm->setCoef0(0);
    this->svm->setDegree(1.0);
    this->trainData = TrainData();
}

ExqClassifier::~ExqClassifier() {
    delete this->svm;
}

std::vector<double> ExqClassifier::train(vector<vector<double>> data, vector<short> labels) {
    //Controller calculates scores and creates the 2D data vector
    this->trainData.data = data;
    this->trainData.labels = labels;
    int rows = this->trainData.data.size();
    int cols = this->trainData.data[0].size();
    //cv::Mat labelsMat(rows, 1, CV_32F);
    cv::Mat labelsMat(rows, 1, CV_32S);
    cv::Mat dataMat(rows, cols, CV_32F);
    for (int i = 0; i < rows; i++) {
        //labelsMat.at<float>(i,0) = labels[i];
        labelsMat.at<short>(i,0) = this->trainData.labels[i];
        for (int j = 0; j < cols; j++) {
            dataMat.at<float>(i,j) = this->trainData.data[i][j];
        }
    }

    this->svm->train(dataMat, cv::ml::ROW_SAMPLE, labelsMat);
    cv::Mat weights = this->svm->getSupportVectors();
    rows = weights.rows;
    cols = weights.cols;
    this->weights.resize(cols);
    if (weights.isContinuous()) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                this->weights[c] += weights.at<float>(r,c);
            }
        }
    }

    return this->weights;
}

inline vector<double> ExqClassifier::getWeights() {
    return this->weights;
}

inline double ExqClassifier::getBias() {
    cv::Mat alpha, svidx;
    return this->svm->getDecisionFunction(0, alpha, svidx);
}
