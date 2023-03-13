//
// Created by Omar Shahbaz Khan on 28/01/2022.
//

#ifndef EXQUISITOR_ITEMPROPERTIES_H
#define EXQUISITOR_ITEMPROPERTIES_H

#include <cstdint>
#include <set>
#include <map>
#include <vector>

namespace exq {
    using std::set;
    using std::map;
    using std::vector;

    // Container for properties
    struct Props {
        // property -> value(s)
        vector<set<uint16_t>> props;
    };

    // Container for properties that have a count value, e.g. number of objects
    struct CountProps {
        // property -> value(s)
        vector<map<uint16_t,uint16_t>> props;
    };

    struct MetaPropsProbs {
        vector<map<int,float>> props;
    };

    /**
     * TODO: Currently supporting items belonging to one collection. Future plan is to support items belonging to
     *       multiple collections.
     * collectionId - id collection item originates from
     * vid - is the item part of a video
     * vidId - id of video the item belongs to
     * std_props - standard Properties for all items (name, aspect ratio, time, etc.)
     * coll_props - collection specific item properties, e.g. Location (LSC), Color (VBS)
     */
    struct ItemProperties {
        uint16_t collectionId = 0;
        bool vid = false;
        int vidId = 0;
        Props stdProps;
        Props collProps;
        CountProps countProps;
    };

    /**
     * TODO: Determine if this data structure is the best approach for Estimated Count
     * collectionIdProb - probability of items being from a collection
     * videoIdProb - probability of items being from a video of a collection
     * stdPropProbs - probabilities of items standard properties
     * collPropProbs - probabilities of items collection properties
     * vidPropProbs - probabilties of items video properties
     */
    struct MetaProbabilities {
        // collection -> probabiltiy
        map<int,float> collectionIdProb;
        // collection -> video -> probability
        vector<map<int,float>> videoIdProb;
        // property -> value(s) -> probability
        MetaPropsProbs stdPropProbs;
        MetaPropsProbs collPropProbs;
        // collection -> video -> property -> value(s) -> probability
        vector<map<int,MetaPropsProbs>> vidPropProbs;
    };
}

#endif //EXQUISITOR_ITEMPROPERTIES_H
