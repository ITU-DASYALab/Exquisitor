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
#include <numeric>
#include <string>
#include <future>

#include "Metadata/ItemProperties.h"
#include "Metadata/ItemFilter.h"
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
            totalItemsConsideredPerSegment = vector<int>(segments, 0);
            std::fill(totalItemsConsideredPerSegment.begin(), totalItemsConsideredPerSegment.end(), 0);
            totalTimePerSegment = vector<double>(segments, 0.0);
            std::fill(totalTimePerSegment.begin(), totalTimePerSegment.end(), 0.0);
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
                int bClusters,
                vector<ExqFunctions<T>*> functions,
                ExqDataHandler<T>* handler,
                vector<ExqClassifier*> classifiers,
                ExqWorker<T>* worker,
                const vector<ItemProperties>& itemProps,
                const vector<vector<Props>>& vidProps,
                vector<double> modWeights,
                bool ffs,
                int guaranteedSlots
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

        int getNumModalites() { return _modalities; }


        /**
         * @brief train the Linear SVM model and select scope (clusters)
         */
        vector<double> train(const vector<uint>& trainIds, const vector<float>& trainLabels,
                             bool changeFilters, Filters filters);

        /**
         * @brief get \a k suggestions from the latest generated SVM model
         */
        TopResults suggest(int k, const vector<uint>& seen, bool changeFilters, const Filters& filters,
                           TopResults prevResults=TopResults(0));

        /**
         * @brief create a new SVM model
         */
        void new_model();

        /**
         * @brief reset the current SVM model
         */
        void reset_model();

        /**
         * @brief set initial modality weights - the lower the weight the less impact it has on rank aggregation
         * @param modWeights
         */
        void set_modality_weights(vector<double> modWeights);

        bool update_modality_weights(vector<uint32_t>& ids, vector<float>& labels);
        void reset_modality_weights();

        vector<ExqArray<pair<int,float>>> get_descriptors(vector<int> ids, int mod);

    private:
        int _iota = 1;
        int _noms = 100;
        int _numWorkers = 1;
        int _segments = 16;
        int _modalities = 1;
        int _bClusters = 256;

        vector<ExqFunctions<T>*> _functions;
        ExqDataHandler<T>* _handler;
        vector<ExqClassifier*> _classifiers;
        ExqWorker<T>* _worker;
        vector<ItemProperties> _itemProperties;
        //collection -> video -> property -> value(s)
        vector<vector<Props>> _vidProperties;
        ItemFilter _activeFilters;

        //TODO: Move this to another class along with sortItems, and assignRanking

        // Original weights of modality
        vector<double> _orgModWeights;
        // Actual weights that are updated based on function
        vector<double> _modalityWeights;
        // Rescaled weights to be between 0 and _orgWeightSum
        vector<double> _rescaledModWeights;
        // Positives from last x rounds
        vector<double> _positivesFromMod;
        int _pref_modality = -1;
        int _update_cnt = 0;
        double _orgWeightSum = 0.0;

        std::map<uint32_t,pair<vector<double>,vector<int>>> _retSuggs = std::map<uint32_t,pair<vector<double>,vector<int>>>();
        double _numPositives = 0.0;

        bool _ffs = false;
        bool _slotsUnset = true;
        int _guaranteedSlots = 0;
        vector<int> _slots;
        vector<int> _activeSlots;
        std::map<uint32_t,pair<int,uint32_t>> _retSuggsFFS = std::map<uint32_t,pair<int,uint32_t>>();

        vector<future<void>> _threads = vector<future<void>>();
        vector<int> _featureDimensions;
        vector<bool> _seenItems;

        vector<bool> _pq_state;

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

    template <typename T>
    inline void ExqController<T>::set_modality_weights(vector<double> modWeights) {
        _modalityWeights = std::move(modWeights);
    }
} //End of namespace exq


#endif //EXQUISITOR_EXQCONTROLLER_H
