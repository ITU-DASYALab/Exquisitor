//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQCONTROLLER_H
#define EXQUISITOR_EXQCONTROLLER_H

#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <future>

#include "ExqConstants.h"
#include "ExqClassifier.h"
#include "ExqFunctions.h"
#include "ExqDataHandler.h"
#include "ExqWorker.h"
//#include <taskflow/taskflow.hpp>

namespace exq {

    using std::array;
    using std::vector;
    using std::string;
    using std::future;

    struct TopResults {
        vector<uint32_t> suggs;
        int* total;
        double* time;
        double overhead_time;
    };
    /**
     * @class ExqController
     * @brief the class that controls the flow of Exquisitor
     */
    template <typename T>
    class ExqController {
    public:
        /**
         * @brief constructs the controller object
         */
        ExqController(
                int iota,
                int noms,
                int workers,
                int segments,
                int numberModalities,
                vector<int> modFeatureDimensions,
                //vector<vector<string>> compFiles,
                //vector<string> cnfgFiles,
                int bClusters,
                ExqFunctions<T>* functions,
                ExqDataHandler<T>* handler,
                ExqClassifier* classifier,
                const ExqWorker* worker
        );

/**
         * @brief destroys the controller object
         */
        ~ExqController();

        /**
         * @brief sets the iota level for compression scheme
         */
        void setIota(int iota);
        /**
         * @brief sets number of items to check per modality before fusion
         */
        void setNominationsPerModality(int noms);
        /**
         * @brief sets the number of worker threads to use
         */
        void setWorkers(int workers);
        /**
         * @brief sets the number of modalities
         */
        void setNumModalities(int modalities);
        /**
         * @brief sets the modality to use, default is 0 which means use all
         */
        void setSegments(int segments);

        /**
         * @brief sets the number of clusters to check during train/suggest phase to b
         */
        void setClusterScope(int b);

        /**
         * @brief train the Linear SVM model
         */
        vector<double> train(vector<uint32_t> trainIds, vector<short> trainLabels);

        /**
         * @brief get \a k suggestions from the latest generated SVM model
         */
        TopResults suggest(int k, vector<uint32_t> seen);

        /**
         * @brief create a new SVM model
         */
        void new_model();

        /**
         * @brief reset the current SVM model
         */
        void reset_model();

    private:
        int _iota;
        int _noms;
        int _numWorkers;
        int _segments;
        int _modalities;
        int _bClusters;

        ExqFunctions<T>* _functions;
        ExqDataHandler<T>* _handler;
        ExqClassifier* _classifier;
        const ExqWorker* _worker;

        vector<future<void>> _threads = vector<future<void>>();
        vector<int> _featureDimensions;
        vector<bool> _seenItems;

        void _init_filters();
        void _init_workers();
        void _print_feature_vec();

    }; //End of class ExqController

    template <typename T>
    inline void ExqController<T>::setIota(int iota) {
        _iota = iota;
    }

    template <typename T>
    inline void ExqController<T>::setNominationsPerModality(int noms) {
        _noms = noms;
    }

    template <typename T>
    inline void ExqController<T>::setNumModalities(int modalities) {
        _modalities = modalities;
    }

    template <typename T>
    inline void ExqController<T>::setWorkers(int numWorkers) {
        _numWorkers = numWorkers;
    }

    template <typename T>
    inline void ExqController<T>::setSegments(int segments) {
        _segments = segments;
    }

    template <typename T>
    inline void ExqController<T>::setClusterScope(int b) {
        _bClusters = b;
    }
} //End of namespace exq


#endif //EXQUISITOR_EXQCONTROLLER_H
