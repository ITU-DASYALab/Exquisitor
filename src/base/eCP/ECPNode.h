#pragma once

#include "ECPConfig.h"
#include "../ExqDescriptor.h"

#include <vector>

namespace exq {

    using std::vector;

    template<typename T, typename U, typename V>
    class ECPNode {
    public:
        int parent = -1;
        ECPNode(ExqDescriptor<T,U,V>* _centroid, bool _addChild = 0);

        ~ECPNode();

        void addChild(ExqDescriptor<T,U,V>* child);

        // Support for direct access
        ExqDescriptor<T,U,V>* getCentroid() { return _centroid; };

        int getNumChildren() { return _children.size(); };

        ExqDescriptor<T,U,V>* get(uint32_t i);

        // Support for scanning
        void open();

        ExqDescriptor<T,U,V>* next();

        void close();

        // Debug
        void PrintNode(string indent = "");

    private:
        // The structure of the node
        ExqDescriptor<T,U,V>* _centroid;
        vector<ExqDescriptor<T,U,V>*> _children;

        // Space is allocated up to maxChildren
        // If more space is needed, it must be reallocated
        int _maxChildren;

        // Support for scanning the node
        int _currChild;
    };
}