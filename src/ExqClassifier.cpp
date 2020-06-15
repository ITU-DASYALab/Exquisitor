//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqClassifier.h"

#include <opencv2/ml.hpp>

using namespace exq;

ExqClassifier::ExqClassifier() {
    this->svm = SVMSGD::create();
    this->svm->setOptimalParameters(SVMSGD::ASGD, SVMSGD::HARD_MARGIN); //Average SGD, Linearly Separable
    this->trainData = TrainData();
}

void ExqClassifier::trainSVM(vector<vector<double>> data, vector<double> labels) {
    //Controller calculates scores and creates the 2D data vector
    this->trainData.data = data;
    this->trainData.labels = labels;
    int rows = data.size();
    int cols = data[0].size();
    cv::Mat labelsMat(rows, 1, CV_32F);
    cv::Mat dataMat(rows, cols, CV_32F);
    for (int i = 0; i < rows; i++) {
        labelsMat.at<float>(i,0) = labels[i];
        for (int j = 0; j < cols; j++) {
            dataMat.at<float>(i,j) = data[i][j];
        }
    }

    this->svm->train(dataMat, cv::ml::ROW_SAMPLE, labelsMat);
}
