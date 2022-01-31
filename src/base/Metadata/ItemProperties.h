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

    struct CollProps {
        ExqArray<ExqArray<uint16_t>> props;
    };

    struct VidProps {
        ExqArray<ExqArray<uint16_t>> props;
    };

    struct ItemProperties {
        uint8_t collections;
        bool vid=false;
        VidProps vid_props;
        CollProps coll_props;
    };
}

#endif //EXQUISITOR_ITEMPROPERTIES_H
