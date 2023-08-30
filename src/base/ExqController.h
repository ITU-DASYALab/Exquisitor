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
#include "IExqFunctions.h"
#include "IExqDataHandler.h"
#include "r64/ExqDescriptorR64.h"
#include "ExqWorker.h"

namespace exq {

    using std::array;
    using std::vector;
    using std::string;
    using std::future;

    struct TopResults {
        vector<uint32_t> suggs;
        vector<int> totalItemsConsideredPerSegment;
        vector<float> totalTimePerSegment;
        float overheadTime;
        TopResults(int segments) {
            suggs = vector<uint32_t>();
            totalItemsConsideredPerSegment = vector<int>(segments, 0);
            std::fill(totalItemsConsideredPerSegment.begin(), totalItemsConsideredPerSegment.end(), 0);
            totalTimePerSegment = vector<float>(segments, 0.0f);
            std::fill(totalTimePerSegment.begin(), totalTimePerSegment.end(), 0.0f);
        }
    };
    /**
     * @class ExqController
     * @brief the class that controls the flow of Exquisitor
     */
    template <class T>
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
                vector<IExqFunctions<T>*> functions,
                IExqDataHandler<T>* handler,
                vector<ExqClassifier*> classifiers,
                ExqWorker<T>* worker,
                const vector<ItemProperties>& itemProps,
                const vector<vector<Props>>& vidProps,
                vector<float> modWeights,
                bool ffs,
                int guaranteedSlots
        );

        ExqController() {};

        /**
         * @brief destroys the controller object
         */
        ~ExqController();


        /**
         * @brief train the Linear SVM model and select scope (clusters)
         */
        vector<float> train(const vector<uint>& trainIds, const vector<float>& trainLabels,
                             bool changeFilters, Filters filters);

        /**
         * @brief get \a k suggestions from the latest generated SVM model
         */
        TopResults suggest(int k, const vector<uint>& seen, bool changeFilters, const Filters& filters,
                           TopResults prevResults=TopResults(0));

        /**
         * @brief create a new SVM model
         * TODO: Need to change SVM models to not just be per modality but a list of SVM's per modality.
         */
        void new_model();

        /**
         * @brief reset the current SVM model
         */
        void reset_model(bool resetSession);

        bool update_modality_weights(vector<uint32_t>& ids, vector<float>& labels);
        void reset_modality_weights();

        vector<ExqArray<pair<int,float>>> get_descriptors(vector<int> ids, int mod);

        /**
         * @brief sets the iota level for compression scheme
         */
        inline void setIota(int iota) {
            _iota = iota;
        }

        /**
         * @brief sets number of items to check per modality before fusion
         */
        inline void setNominationsPerModality(int noms) {
            _noms = noms;
        }

        /**
         * @brief sets the number of modalities
         */
        inline void setNumModalities(int modalities) {
            _modalities = modalities;
        }

        /**
         * @brief sets the number of worker threads to use
         */
        inline void setWorkers(int numWorkers) {
            _numWorkers = numWorkers;
        }

        /**
         * @brief sets the modality to use, default is 0 which means use all
         */
        inline void setSegments(int segments) {
            _segments = segments;
        }

        /**
         * @brief sets the number of clusters to check during train/suggest phase to b
         */
        inline void setClusterScope(int b) {
            _bClusters = b;
        }

        /**
         * @brief set initial modality weights - the lower the weight the less impact it has on rank aggregation
         */
        inline void set_modality_weights(vector<float> modWeights) {
            _modalityWeights = std::move(modWeights);
        }

        inline int getNumModalites() { return _modalities; }

    private:
        int _iota = 1;
        int _noms = 100;
        int _numWorkers = 1;
        int _segments = 16;
        int _modalities = 1;
        int _bClusters = 256;

        vector<IExqFunctions<T>*> _functions;
        IExqDataHandler<T>* _handler;
        vector<ExqClassifier*> _classifiers;
        ExqWorker<T>* _worker;
        vector<ItemProperties> _itemProperties;
        //collection -> video -> property -> value(s)
        vector<vector<Props>> _vidProperties;
        ItemFilter _activeFilters;

        //TODO: Move this to another class along with sortItems, and assignRanking

        // Original weights of modality
        vector<float> _orgModWeights;
        // Actual weights that are updated based on function
        vector<float> _modalityWeights;
        // Rescaled weights to be between 0 and _orgWeightSum
        vector<float> _rescaledModWeights;
        // Positives from last x rounds
        vector<float> _positivesFromMod;
        int _pref_modality = -1;
        int _update_cnt = 0;
        float _orgWeightSum = 0.0f;

        std::map<uint32_t,pair<vector<float>,vector<int>>> _retSuggs = std::map<uint32_t,pair<vector<float>,vector<int>>>();
        float _numPositives = 0.0f;

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

        // void _init_filters();
        // void _init_workers();

    }; //End of class ExqController

} //End of namespace exq


#endif //EXQUISITOR_EXQCONTROLLER_H
