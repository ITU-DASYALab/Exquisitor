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
    };
    class ItemFilter {
    public:
        ItemFilter(Filters filters);
        bool compare(ItemProperties item, Props& vidProps);

    private:
        Filters _filters;
    };
}


#endif //EXQUISITOR_ITEMFILTER_H
