//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQCONTROLLER_H
#define EXQUISITOR_EXQCONTROLLER_H

#pragma once

#include <cstdint>
#include <inttypes.h>
#include <array>
#include <vector>
#include <string>
#include <future>

#include "Metadata/ItemProperties.h"
#include "Metadata/ItemFilter.h"
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
        vector<int> totalItemsConsideredPerSegment;
        vector<double> totalTimePerSegment;
        double overheadTime;
        TopResults(int segments) {
            suggs = vector<uint32_t>();
            totalItemsConsideredPerSegment = vector<int>(segments);
            totalTimePerSegment = vector<double>(segments);
        }
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
                vector<ExqFunctions<T>*> functions,
                ExqDataHandler<T>* handler,
                vector<ExqClassifier*> classifiers,
                ExqWorker<T>* worker,
                const vector<ItemProperties>& itemProps,
                const vector<vector<Props>>& vidProps
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
        vector<double> train(const vector<uint>& trainIds, const vector<float>& trainLabels,
                             bool changeFilters, Filters filters);//Filters filters);
        //vector<double> train(const vector<uint>& trainIds, const vector<float>& trainLabels,
        //                     bool changeFilters,
        //                     set<uint16_t> collections,
        //                     vector<pair<int,set<uint16_t>>> stdFilters,
        //                     vector<pair<int,vector<pair<int,set<uint16_t>>>>> collFilters,
        //                     vector<pair<int,vector<pair<int,set<uint16_t>>>>> vidFilters);

        /**
         * @brief get \a k suggestions from the latest generated SVM model
         */
        TopResults suggest(int k, const vector<uint>& seen, bool useActiveFilters, const Filters& filters);

        /**
         * @brief create a new SVM model
         */
        void new_model();

        /**
         * @brief reset the current SVM model
         */
        void reset_model();

        vector<ExqArray<pair<int,float>>> get_descriptors(vector<int> ids, int mod);

    private:
        int _iota;
        int _noms;
        int _numWorkers;
        int _segments;
        int _modalities;
        int _bClusters;

        vector<ExqFunctions<T>*> _functions;
        ExqDataHandler<T>* _handler;
        vector<ExqClassifier*> _classifiers;
        ExqWorker<T>* _worker;
        vector<ItemProperties> _itemProperties;
        //collection -> video -> property -> value(s)
        vector<vector<Props>> _vidProperties;
        ItemFilter _activeFilters;

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
