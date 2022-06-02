//
// Created by Omar Shahbaz Khan on 13/12/2021.
//

#ifndef EXQUISITOR_ITEMFILTER_H
#define EXQUISITOR_ITEMFILTER_H

#include "ItemProperties.h"

#include <set>
#include <array>

namespace exq {
    using std::set;
    using std::map;
    using std::pair;
    using std::array;

    enum Operator {
        EQ=0,
        GTE=1,
        LTE=2,
        RNG=3
    };

    /**
     * TODO: Currently supporting items belonging to one collection. Future plan is to support items belonging to
     *       multiple collections.
     * TODO: Probably better to use a pair of sets (eq,neq)
     */
    struct Filters {
        // value -> operator
        set<uint16_t> collection;
        //map<int,set<uint16_t>> videos;
        // property -> values
        vector<pair<int,set<uint16_t>>> stdFilters;
        // vec[0]: (collectionId, properties[0]: (property,values))
        vector<pair<int,vector<pair<int,set<uint16_t>>>>> collFilters;
        set<uint16_t> video;
        vector<pair<int,vector<pair<int,set<uint16_t>>>>> vidFilters;
        set<uint16_t> negCollection;
        vector<pair<int,set<uint16_t>>> negStdFilters;
        vector<pair<int,vector<pair<int,set<uint16_t>>>>> negCollFilters;
        set<uint16_t> negVideo;
        vector<pair<int,vector<pair<int,set<uint16_t>>>>> negVidFilters;
        // Range (array - 0 = operator, 1 = value0, 2 = value1 - value2 is only checked if operator == RNG)
        vector<pair<int,vector<pair<int,map<uint16_t,array<uint16_t,3>>>>>> rangeFilters;
        Filters() {
            collection = set<uint16_t>();
            stdFilters = vector<pair<int,set<uint16_t>>>();
            collFilters = vector<pair<int,vector<pair<int,set<uint16_t>>>>>();
            video = set<uint16_t>();
            vidFilters = vector<pair<int,vector<pair<int,set<uint16_t>>>>>();
            negCollection = set<uint16_t>();
            negStdFilters = vector<pair<int,set<uint16_t>>>();
            negCollFilters = vector<pair<int,vector<pair<int,set<uint16_t>>>>>();
            negVideo = set<uint16_t>();
            negVidFilters = vector<pair<int,vector<pair<int,set<uint16_t>>>>>();
            rangeFilters = vector<pair<int,vector<pair<int,map<uint16_t,array<uint16_t,3>>>>>>();
        }
        ~Filters() {
            collection.clear();
            negCollection.clear();
            video.clear();
            negVideo.clear();
            for (auto e : stdFilters)
                e.second.clear();
            stdFilters.clear();
            for (auto e : negStdFilters)
                e.second.clear();
            negStdFilters.clear();
            for (auto c : collFilters) {
                for (auto e: c.second)
                    e.second.clear();
                c.second.clear();
            }
            collFilters.clear();
            for (auto c : negCollFilters) {
                for (auto e: c.second)
                    e.second.clear();
                c.second.clear();
            }
            negCollFilters.clear();
            for (auto c : vidFilters) {
                for (auto e: c.second)
                    e.second.clear();
                c.second.clear();
            }
            vidFilters.clear();
            for (auto c : negVidFilters) {
                for (auto e: c.second)
                    e.second.clear();
                c.second.clear();
            }
            negVidFilters.clear();
            for (auto c : rangeFilters) {
                for (auto e: c.second)
                    e.second.clear();
                c.second.clear();
            }
            rangeFilters.clear();
        }
    };
    class ItemFilter {
    public:
        explicit ItemFilter(Filters filters);
        ItemFilter() { _filters = Filters(); };
        ~ItemFilter();
        bool compare(ItemProperties& item, vector<vector<Props>>& vidProps);
        inline void setFilters(Filters filters) {
            _filters.collection = filters.collection;
            _filters.stdFilters = filters.stdFilters;
            _filters.collFilters = filters.collFilters;
            _filters.video = filters.video;
            _filters.vidFilters = filters.vidFilters;
            _filters.negCollection = filters.negCollection;
            _filters.negStdFilters = filters.negStdFilters;
            _filters.negCollFilters = filters.negCollFilters;
            _filters.negVideo = filters.negVideo;
            _filters.negVidFilters = filters.negVidFilters;
            _filters.rangeFilters = filters.rangeFilters;
        };

    private:
        Filters _filters;
    };
}


#endif //EXQUISITOR_ITEMFILTER_H
