//
// Created by Omar Shahbaz Khan on 13/12/2021.
//

#include "ItemFilter.h"

using namespace exq;

ItemFilter::ItemFilter(Filters filters) {
    _filters = filters;
}

bool ItemFilter::compare(ItemProperties item, Props &vidProps) {
    // Check collection id
    if (!_filters.collection.empty())
        if (!_filters.collection.contains(item.collectionId))
            return false;
    // Check standard properties
    if (!_filters.stdFilters.empty()) {
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
        for (auto c : _filters.collFilters) {
            // As it is treated as an OR filter, item has already passed collection id filter
            if (!_filters.collection.empty())
                if (item.collectionId != c.first)
                    continue;
            if (item.collectionId != c.first)
                return false;
            for (auto prop : c.second) {
                bool ok = false;
                for (auto elem : item.collProps.props[prop.first]) {
                    if (prop.second.contains(elem)) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) return false;
            }
        }
    }
    // Check video properties
    if (!_filters.vidFilters.empty()) {
        for (auto c : _filters.vidFilters) {
            // As it is treated as an OR filter, item has already passed collection id filter
            if (!_filters.collection.empty())
                if (item.collectionId != c.first)
                    continue;
            if (item.collectionId != c.first)
                return false;
            for (auto prop : c.second) {
                bool ok = false;
                for (auto elem : vidProps.props[prop.first]) {
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