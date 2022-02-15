//
// Created by Omar Shahbaz Khan on 13/12/2021.
//

#ifndef EXQUISITOR_ITEMFILTER_H
#define EXQUISITOR_ITEMFILTER_H

#include "ItemProperties.h"

namespace exq {
    class ItemFilter {
    public:
        virtual bool compare() = 0;
    };

    class LSC_ItemFilter : public ItemFilter {
    public:
        bool compare() override;

    private:
        int day;
        int hour;
        int location;
    };

    class VBS_ItemFilter : public ItemFilter {
    public:
        bool compare() override;

    private:
        int video;
        int faces;
        int color;
        int text;
        //ExqArray<int> ctgs;
        //ExqArray<int> tags;
    };
}


#endif //EXQUISITOR_ITEMFILTER_H
