//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#include "ExqClassifier.h"

#include <opencv2/ml.hpp>

using namespace exq;
using std::cout;
using std::endl;

ExqClassifier::ExqClassifier() {
    //this->svmsgd->setOptimalParameters(SVMSGD::ASGD, SVMSGD::HARD_MARGIN); //Average SGD, Linearly Separable
    //this->svmsgd->setMarginRegularization(0.01);
    this->svm = SVM::create();
    this->svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 0.01));
    this->svm->setKernel(SVM::LINEAR);
    this->trainData = TrainData();
}

std::vector<double> ExqClassifier::trainSVM(vector<vector<double>> data, vector<double> labels) {
    //Controller calculates scores and creates the 2D data vector
    this->trainData.data = data;
    this->trainData.labels = labels;
    int rows = data.size();
    int cols = data[0].size();
    //cv::Mat labelsMat(rows, 1, CV_32F);
    cv::Mat labelsMat(rows, 1, CV_32S);
    cv::Mat dataMat(rows, cols, CV_32F);
    for (int i = 0; i < rows; i++) {
        //labelsMat.at<float>(i,0) = labels[i];
        labelsMat.at<short>(i,0) = labels[i];
        for (int j = 0; j < cols; j++) {
            dataMat.at<float>(i,j) = data[i][j];
        }
    }

    //this->svmsgd->train(dataMat, cv::ml::ROW_SAMPLE, labelsMat);
    this->svm->train(dataMat, cv::ml::ROW_SAMPLE, labelsMat);
    //cout << "Shift: " << this->svm->getShift() << endl;
    //cv::Mat weights = this->svmsgd->getWeights();
    cv::Mat weights = this->svm->getSupportVectors();
    //cout << "Rows " << weights.rows << endl;
    //cout << "Cols " << weights.cols << endl;
    vector<double> wVector = vector<double>();
    if (weights.isContinuous()) {
        wVector.assign(weights.begin<float>(), weights.end<float>());
    }

    return wVector;
}
