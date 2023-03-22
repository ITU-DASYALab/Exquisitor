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

        inline void addChild(ExqDescriptorR64* child) {
            // Now there is enough space, so insert
            _children.push_back(child);
        };

        inline ExqDescriptorR64* get(uint32_t i) {
            if (i > _children.size()) {
                return NULL;
            }
            return _children[i];
        }

        inline void open() {
            _currChild = 0;
        }

        inline ExqDescriptorR64* next() {
            if (_currChild < (int)_children.size()) {
                return _children[_currChild++];
            }
            return NULL;
        }

        inline void close() {
            _currChild = -1;
        }

        // For debugging/logging purposes
        inline void PrintNode(string indent) {
            cout << indent << "NODE: Centroid: ";
            //PrintDescriptor(centroid);

            cout << indent << "NODE: numChildren = " << _children.size() << " (maxChildren = " << _maxChildren << ")" << endl;
            for (int i = 0; i < (int)_children.size(); i++) {
                cout << indent << "NODE: Child " << i << ": ";
                //PrintDescriptor(children[i]);
            }
        }

        // Support for direct access
        inline ExqDescriptorR64* getCentroid() { return _centroid; };

        inline int getNumChildren() { return _children.size(); };

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