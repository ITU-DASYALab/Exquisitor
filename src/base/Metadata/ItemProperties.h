//
// Created by Omar Shahbaz Khan on 28/01/2022.
//

#ifndef EXQUISITOR_ITEMPROPERTIES_H
#define EXQUISITOR_ITEMPROPERTIES_H

#include <cstdint>
#include <set>
#include <map>

#include "../ExqArray.h"

namespace exq {
    using std::set;
    using std::map;

    // Container for properties
    struct Props {
        ExqArray<ExqArray<uint16_t>> props = ExqArray<ExqArray<uint16_t>>();
    };

    /**
     * collectionId - id collection item originates from
     * vid - is the item part of a video
     * vidId - id of video the item belongs to
     * std_props - standard Properties for all items (name, aspect ratio, time, etc.)
     * coll_props - collection specific item properties, e.g. Location (LSC), Color (VBS)
     */
    struct ItemProperties {
        uint8_t collectionId = 0;
        bool vid = false;
        int vidId = 0;
        Props std_props;
        Props coll_props;
    } ItemProperties;
}

#endif //EXQUISITOR_ITEMPROPERTIES_H
