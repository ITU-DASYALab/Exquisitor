//
// Created by Omar Shahbaz Khan on 17/06/2020.
//

#include "gtest/gtest.h"
#include "base/ExqClassifier.h"
#include "base/r64/ExqFunctionsR64.h"
#include "base/r64/ExqDescriptorR64.h"
#include "base/IExqDescriptor.h"
#include "base/ExqArray.h"

using exq::ExqClassifier;
using exq::ExqFunctionsR64;
using exq::ExqDescriptorR64;
using exq::IExqDescriptor;
using exq::ExqArray;
using std::cout;
using std::endl;
using std::vector;
using std::pair;

TEST (SvmModel, trainModel) {
    const float POS_L = 1.0;
    const float NEG_L = -1.0;
    vector<vector<double>> trainingItems;
    vector<float> trainingItemLabels;
    auto classifier = new ExqClassifier(12988);
    auto functions = new ExqFunctionsR64(5, 1, 48, 16, 16, 281474976710655,
                                         200000000000000.0, 65535, 65535, 50000.0);

    vector<IExqDescriptor<uint64_t>*> pos = vector<IExqDescriptor<uint64_t>*>();
    auto fids = ExqArray<uint64_t>(1);
    auto frat = ExqArray<uint64_t>(1);
    fids.setItem(2502350455216022170U,0);
    frat.setItem(12141593397245575683U,0);
    pos.push_back(new ExqDescriptorR64(905584, 2489824311291099305U, fids, frat));
    fids.setItem(2487151180132000371U,0);
    frat.setItem(13647243195524072278U,0);
    pos.push_back(new ExqDescriptorR64(160763, 2489697280686021561U, fids, frat));


    vector<IExqDescriptor<uint64_t>*> neg = vector<IExqDescriptor<uint64_t>*>();
    fids.setItem(2655207976273320278U,0);
    frat.setItem(12915398048199234038U,0);
    neg.push_back(new ExqDescriptorR64(881441, 2794240416452107909U, fids, frat));
    fids.setItem(2784108933978589896U,0);
    frat.setItem(14049841615536998960U,0);
    neg.push_back(new ExqDescriptorR64(992166, 1407194997803292061U, fids, frat));

    for (int i = 0; i < pos.size() + neg.size(); i++) {
        if (i < pos.size()) {
            trainingItemLabels.push_back(POS_L);
        }
        if (i >= pos.size() && i < pos.size() + neg.size()) {
            trainingItemLabels.push_back(NEG_L);
        }
    }

    for (int i = 0; i < pos.size(); i++) {
        ExqArray<pair<int, float>> descVals = functions->getDescriptorInformation(*pos[i]);
        vector<double> featVals = vector<double>();
        for (int j = 0; j < 12988; j++) {
            featVals.push_back(0.0);
        }
        for (int j = 0; j < descVals.getSize(); j++) {
            pair<int, float> item = descVals.getItem(j);
            cout << "Feat Id " << item.first << endl;
            cout << "Val " << item.second << endl;
            featVals[item.first] = item.second;
        }
        trainingItems.push_back(featVals);
    }
    for (int i = 0; i < neg.size(); i++) {
        ExqArray<pair<int, float>> descVals = functions->getDescriptorInformation(*neg[i]);
        vector<double> featVals = vector<double>();
        for (int j = 0; j < 12988; j++) {
            featVals.push_back(0.0);
        }
        for (int j = 0; j < descVals.getSize(); j++) {
            pair<int, float> item = descVals.getItem(j);
            cout << "Feat Id " << item.first << endl;
            cout << "Val " << item.second << endl;
            featVals[item.first] = item.second;
        }
        trainingItems.push_back(featVals);
    }

    auto weights = classifier->train(trainingItems, trainingItemLabels);
    auto bias = classifier->getBias();
    //8845 is most positive feature - Should always be highest
    cout << "P " << weights[8845] << endl;
    for (int i = 0; i < weights.size(); i++) {
        if (weights[i] != 0.0) {
            cout << i << " " << weights[i] << endl;
            double w = weights[i];
            assert(weights[8845] >= w);
        }
    }

//    //2760 -0.594964
//    cout << "N " << weights[2760] << endl;
//    assert(weights[2760] != 0.0);
//    //4999 -1.56635
//    cout << "N " << weights[4999] << endl;
//    assert(weights[4999] != 0.0);
//    //6486 -1.13351
//    cout << "N " << weights[6486] << endl;
//    assert(weights[6486] != 0.0);
//    //6601 -0.452613
//    cout << "N " << weights[6601] << endl;
//    assert(weights[6601] != 0.0);
//    //8819 1.08677
//    cout << "P " << weights[8819] << endl;
//    assert(weights[8819] != 0.0);
//    //8822 0.448359
//    cout << "P " << weights[8822] << endl;
//    assert(weights[8822] != 0.0);
//    //8827 0.00279007
//    cout << "P " << weights[8827] << endl;
//    assert(weights[8827] != 0.0);
//    //8836 0.426009
//    cout << "P " << weights[8836] << endl;
//    assert(weights[8836] != 0.0);
//    //8858 0.00404155
//    cout << "P " << weights[8858] << endl;
//    assert(weights[8858] != 0.0);
//    //8880 0.00351882
//    cout << "P " << weights[8880] << endl;
//    assert(weights[8880] != 0.0);
//    //8890 0.002407
//    cout << "P " << weights[8890] << endl;
//    assert(weights[8890] != 0.0);
//    //8914 0.439329
//    cout << "P " << weights[8914] << endl;
//    assert(weights[8914] != 0.0);
//    //9299 -0.0382548
//    cout << "N " << weights[9299] << endl;
//    assert(weights[9299] != 0.0);
//    //9433 -0.773048
//    cout << "N " << weights[9433] << endl;
//    assert(weights[9433] != 0.0);
//    //9870 -0.903546
//    cout << "N " << weights[9870] << endl;
//    assert(weights[9870] != 0.0);
//    //9891 -0.0381898
//    cout << "N " << weights[9891] << endl;
//    assert(weights[9891] != 0.0);
//    //9927 -2.70116
//    cout << "N " << weights[9927] << endl;
//    assert(weights[9927] != 0.0);
//    //12694 -0.842394
//    cout << "N " << weights[12694] << endl;
//    assert(weights[12694] != 0.0);
}
