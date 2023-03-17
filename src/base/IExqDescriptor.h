
#ifndef EXQUISITOR_EXQDESCRIPTOR_H
#define EXQUISITOR_EXQDESCRIPTOR_H
#pragma once

#include <cstdint>
#include <cinttypes>

namespace exq {
    /**
     * @class IExqDescriptor
     * @brief Interface representing a feature descriptor
     */
    template <class T>
    class IExqDescriptor {
        public:
        uint32_t _id;

        IExqDescriptor() {}

        ~IExqDescriptor() {}

        virtual void setItemId(uint32_t itemId) { _id = itemId; }

        virtual uint32_t getId() { return _id; }

        virtual T getTop() = 0;

        virtual ExqArray<T> getFeatureIds() = 0;

        virtual ExqArray<T> getFeatureRatios() = 0;

    };

} // namespace exq

#endif // EXQUISITOR_EXQDESCRIPTOR_H