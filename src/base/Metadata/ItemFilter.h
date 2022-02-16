//
// Created by Omar Shahbaz Khan on 13/12/2021.
//

#ifndef EXQUISITOR_ITEMFILTER_H
#define EXQUISITOR_ITEMFILTER_H

#include "ItemProperties.h"

#include <set>

namespace exq {
    using std::set;
    using std::pair;

    /**
     * TODO: Currently supporting items belonging to one collection. Future plan is to support items belonging to
     *       multiple collections.
     */
    struct Filters {
        set<uint16_t> collection;
        //map<int,set<uint16_t>> videos;
        // property -> values
        vector<pair<int,set<uint16_t>>> stdFilters;
        // vec[0]: (collectionId, properties[0]: (property,values))
        vector<pair<int,vector<pair<int,set<uint16_t>>>>> collFilters;
        vector<pair<int,vector<pair<int,set<uint16_t>>>>> vidFilters;
        Filters() {
            collection = set<uint16_t>();
            stdFilters = vector<pair<int,set<uint16_t>>>();
            collFilters = vector<pair<int,vector<pair<int,set<uint16_t>>>>>();
            vidFilters = vector<pair<int,vector<pair<int,set<uint16_t>>>>>();
        }
        ~Filters() {
            collection.clear();
            for (auto e : stdFilters)
                e.second.clear();
            stdFilters.clear();
            for (auto c : collFilters) {
                for (auto e: c.second)
                    e.second.clear();
                c.second.clear();
            }
            collFilters.clear();
            for (auto c : vidFilters) {
                for (auto e: c.second)
                    e.second.clear();
                c.second.clear();
            }
            vidFilters.clear();
        }
    };
    class ItemFilter {
    public:
        explicit ItemFilter(Filters filters);
        ItemFilter() { _filters = Filters(); };
        ~ItemFilter();
        bool compare(ItemProperties item, Props& vidProps);
        inline void setFilters(Filters filters) {
            _filters.collection = filters.collection;
            _filters.stdFilters = filters.stdFilters;
            _filters.collFilters = filters.collFilters;
            _filters.vidFilters = filters.vidFilters;
        };

    private:
        Filters _filters;
    };
}


#endif //EXQUISITOR_ITEMFILTER_H
