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

#include "ExqConstants.h"
#include "ExqClassifier.h"
#include "IExqFunctions.h"
//#include <taskflow/taskflow.hpp>

namespace exq {

    using std::array;
    using std::vector;
    using std::string;

    /**
     * @class ExqController
     * @brief the class that controls the flow of Exquisitor
     */
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
                vector<bool> activeModalities,
                vector<int> modFeatureDimensions,
                vector<vector<string>> compFiles,
                vector<string> cnfgFiles,
                int bClusters
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
         * @brief sets the modality to use, default is 0 which means use all
         */
        void setModActivity(int modActivity);
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
        double* train(uint32_t* newXI, double* newY, int nNewTrain);

        /**
         * @brief get \a k suggestions from the latest generated SVM model
         */
        void suggest(int k);

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
        int _modActivity;
        int _workers;
        int _segments;

        IExqFunctions* _functions;

        vector<bool> _seenItems;

        void _init_filters();
        void _init_workers();
        void _print_feature_vec();

    }; //End of class ExqController

    inline void ExqController::setIota(int iota) {
        _iota = iota;
    }

    inline void ExqController::setNominationsPerModality(int noms) {
        _noms = noms;
    }

    inline void ExqController::setModActivity(int modActivity) {
        _modActivity = modActivity;
    }

    inline void ExqController::setWorkers(int workers) {
        _workers = workers;
    }

    inline void ExqController::setSegments(int segments) {
        _segments = segments;
    }

} //End of namespace exq


#endif //EXQUISITOR_EXQCONTROLLER_H
