#pragma once

#include "ECPConfig.h"
#include "../r64/ExqDescriptorR64.h"

#include <vector>

namespace exq {

    using std::vector;

    class ECPNode {
    public:
        int parent = -1;
        ECPNode(ExqDescriptorR64* _centroid, bool _addChild = 0);

        ~ECPNode();

        void addChild(ExqDescriptorR64* child);

        // Support for direct access
        ExqDescriptorR64* getCentroid() { return _centroid; };

        int getNumChildren() { return _children.size(); };

        ExqDescriptorR64* get(uint32_t i);

        // Support for scanning
        void open();

        ExqDescriptorR64* next();

        void close();

        // Debug
        void PrintNode(string indent = "");

    private:
        // The structure of the node
        ExqDescriptorR64* _centroid;
        vector<ExqDescriptorR64*> _children;

        // Space is allocated up to maxChildren
        // If more space is needed, it must be reallocated
        int _maxChildren;

        // Support for scanning the node
        int _currChild;
    };
}