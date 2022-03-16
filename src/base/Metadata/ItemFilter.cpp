//
// Created by Omar Shahbaz Khan on 13/12/2021.
//

#include "ItemFilter.h"

#include <iostream>

using namespace exq;
using std::cout;
using std::endl;

ItemFilter::ItemFilter(Filters filters) {
    _filters.collection = filters.collection;
    _filters.stdFilters = filters.stdFilters;
    _filters.collFilters = filters.collFilters;
    _filters.vidFilters = filters.vidFilters;
}

ItemFilter::~ItemFilter() {}

bool ItemFilter::compare(ItemProperties& item, vector<vector<Props>>& vidProps) {
    // Check collection id
    if (!_filters.collection.empty()) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
        cout << "Collection filters found" << endl;
#endif
        if (!_filters.collection.contains(item.collectionId))
            return false;
    }
    // Check standard properties
    if (!_filters.stdFilters.empty()) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
        cout << "Standard filters found" << endl;
#endif
        // Treat as OR filter
        for (auto prop: _filters.stdFilters) {
            bool ok = false;
            for (auto elem : item.stdProps.props[prop.first]) {
                if (prop.second.contains(elem)) {
                    ok = true;
                    break;
                }
            }
            if (!ok) return false;
        }
    }
    // Check collection properties
    if (!_filters.collFilters.empty()) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
        cout << "Collection property filters found" << endl;
#endif
        for (auto c : _filters.collFilters) {
            //if (!_filters.collection.empty())
            //    if (item.collectionId != c.first)
            //        continue;
            //if (item.collectionId != c.first)
            //    return false;
            for (auto prop : c.second) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "Checking prop " << prop.first << " (item has ";
#endif
                bool ok = false;
                for (auto elem : item.collProps.props[prop.first]) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                    cout << elem << ", ";
#endif
                    if (prop.second.contains(elem)) {
                        ok = true;
                        break;
                    }
                }
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << ") | " << ok << endl;
#endif
                if (!ok) return false;
            }
        }
    }
    // Check video properties
    if (!_filters.vidFilters.empty()) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
        cout << "Video property filters found" << endl;
#endif
        for (auto c : _filters.vidFilters) {
            // As it is treated as an OR filter, item has already passed collection id filter
            if (!_filters.collection.empty())
                if (item.collectionId != c.first) continue;
            if (item.collectionId != c.first) return false;
            if (!item.vid) return false;
            for (auto prop : c.second) {
                bool ok = false;
                for (auto elem : vidProps[item.collectionId][item.vidId].props[prop.first]) {
                    if (prop.second.contains(elem)) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) return false;
            }
        }
    }
    return true;
}