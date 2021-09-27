#pragma once

#include "ECPConfig.h"
#include "base/ExqDescriptor.h"

#include <vector>

namespace exq {

    using std::vector;

    template<typename T, typename U, typename V>
    class ECPNode {
    public:
        ECPNode(ExqDescriptor<T,U,V>* _centroid, bool _addChild = 0);

        ~ECPNode();

        void addChild(ExqDescriptor<T,U,V>* child);

        // Support for direct access
        ExqDescriptor<T,U,V>* getCentroid() { return centroid; };

        int getNumChildren() { return numChildren; };

        ExqDescriptor<T,U,V>* get(uint64_t i);

        // Support for scanning
        void open();

        ExqDescriptor<T,U,V>* next();

        void close();

        // Debug
        void PrintNode(string indent = "");

    private:
        // The structure of the node
        ExqDescriptor <T, U, V> *centroid;
        vector<ExqDescriptor < T, U, V>*> children;
        int numChildren;

        // Space is allocated up to maxChildren
        // If more space is needed, it must be reallocated
        int maxChildren;

        // Support for scanning the node
        int currChild;
    };
}