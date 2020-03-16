//
// Created by Omar Shahbaz Khan on 16/03/2020.
//

#ifndef EXQUISITOR_IEXQDESCRIPTOR_H
#define EXQUISITOR_IEXQDESCRIPTOR_H

#pragma once

#include <iostream>
#include <cstdio>

//TODO: Future work - Interface for Descriptors
namespace exq {

    /**
     * @class Descriptor
     * @brief the class that represents features
     */
    template <typename T, typename U, typename V>
    class IExqDescriptor {
    public:
        virtual ~IExqDescriptor() {};

        virtual int getTopFeature() = 0;

        virtual int getFeature(int i) = 0;

        virtual float getRatio(int i) = 0;

        virtual void printDescriptor();

    protected:
        uint32_t id;
        T topFeature;
        U featureIds;
        V featureRatios;
    }; //End of class ExqDescriptor

} //End of namespace exq


#endif //EXQUISITOR_IEXQDESCRIPTOR_H
