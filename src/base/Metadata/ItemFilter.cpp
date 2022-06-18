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
    _filters.negCollection = filters.negCollection;
    _filters.stdFilters = filters.stdFilters;
    _filters.negStdFilters = filters.negStdFilters;
    _filters.collFilters = filters.collFilters;
    _filters.negCollFilters = filters.negCollFilters;
    _filters.video = filters.video;
    _filters.negVideo = filters.negVideo;
    _filters.vidFilters = filters.vidFilters;
    _filters.negVidFilters = filters.negVidFilters;
    _filters.rangeFilters = filters.rangeFilters;
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
    } else if (!_filters.negCollection.empty()) {
        if (_filters.collection.contains(item.collectionId))
            return false;
    }
    //Check video id
    if (!_filters.video.empty()) {
        if (!_filters.video.contains(item.vidId))
            return false;
    } else if (!_filters.negVideo.empty()) {
        if (_filters.negVideo.contains(item.vidId))
            return false;
    }
    // Check standard properties
    if (!_filters.stdFilters.empty()) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
        cout << "Standard filters found" << endl;
#endif
        // Treat as OR filter
        for (auto prop: _filters.stdFilters) {
            bool found = false;
            for (auto elem : item.stdProps.props[prop.first]) {
                if (prop.second.contains(elem)) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
    } else if (!_filters.negStdFilters.empty()) {
        // Treat as OR filter
        for (auto prop: _filters.negStdFilters) {
            bool found = false;
            for (auto elem : item.stdProps.props[prop.first]) {
                if (prop.second.contains(elem)) {
                    found = true;
                    break;
                }
            }
            if (found) return false;
        }
    }

    // Check collection properties
    if (!_filters.collFilters.empty()) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
        cout << "Collection property filters found" << endl;
#endif
        for (auto c : _filters.collFilters) {
            for (auto prop : c.second) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "Checking prop " << prop.first << " (item has ";
#endif
                bool found = false;
                for (auto elem : item.collProps.props[prop.first]) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                    cout << elem << ", ";
#endif
                    if (prop.second.contains(elem)) {
                        found = true;
                        break;
                    }
                }
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << ") | " << ok << endl;
#endif
                if (!found) return false;
            }
        }
    } else if (!_filters.negCollFilters.empty()) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
        cout << "Collection property filters found" << endl;
#endif
        for (auto c: _filters.negCollFilters) {
            for (auto prop: c.second) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << "Checking prop " << prop.first << " (item has ";
#endif
                bool found = false;
                for (auto elem: item.collProps.props[prop.first]) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                    cout << elem << ", ";
#endif
                    if (prop.second.contains(elem)) {
                        found = true;
                        break;
                    }
                }
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
                cout << ") | " << ok << endl;
#endif
                if (found) return false;
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
                bool found = false;
                for (auto elem : vidProps[item.collectionId][item.vidId].props[prop.first]) {
                    if (prop.second.contains(elem)) {
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }
        }
    } else if (!_filters.negVidFilters.empty()) {
#if defined(DEBUG_EXTRA) || defined(DEBUG_SUGGEST_EXTRA)
        cout << "Video property filters found" << endl;
#endif
        for (auto c: _filters.negVidFilters) {
            // As it is treated as an OR filter, item has already passed collection id filter
            if (!_filters.collection.empty())
                if (item.collectionId != c.first) continue;
            if (item.collectionId != c.first) return false;
            if (!item.vid) return false;
            for (auto prop: c.second) {
                bool found = false;
                for (auto elem: vidProps[item.collectionId][item.vidId].props[prop.first]) {
                    if (prop.second.contains(elem)) {
                        found = true;
                        break;
                    }
                }
                if (found) return false;
            }
        }
    }

    // Check range filters (treated as an and filter)
    if (!_filters.rangeFilters.empty()) {
        //cout << "Checking Range Filters" << endl;
        for (const auto& c: _filters.rangeFilters) { // Collections
            for (auto prop: c.second) { // Filters
                //cout << "prop.first: " << prop.first << endl;
                if (item.countProps.props[prop.first].empty()) {
                    for (auto const&[key, val]: prop.second) {
                        if (val[0] == EQ && val[1] != 0) return false;
                    }
                    continue;
                }
                for (auto const& [key, val] : item.countProps.props[prop.first]) { // Items
                    //cout << "key: " << key << ", val: " << val << endl;
                    if (prop.second.contains(key)) {
                        auto op = prop.second[key][0];
                        //cout << "op = " << op << ", v0 = " << prop.second[key][1] << endl;
                        if (op == EQ && val != prop.second[key][1]) {
                            return false;
                        } else if (op == GTE && val < prop.second[key][1]) {
                            return false;
                        } else if (op == LTE && val > prop.second[key][1]) {
                            return false;
                        } else if (op == RNG && (val < prop.second[key][1] || val > prop.second[key][2])) {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}