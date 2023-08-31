#include "ECPNode.h"
#include <cmath>

using namespace exq;

ECPNode::ECPNode(ExqDescriptorR64* centroid, bool _addChild) {
    // Note the centroid
    _centroid = centroid;

    // Allocate space for children
    _maxChildren = 200;
    _children = vector<ExqDescriptorR64*>();
    _children.reserve(_maxChildren);

    if (_addChild)
        addChild(new ExqDescriptorR64(_centroid));
};

ECPNode::~ECPNode() {
    delete _centroid;
    for (int i = 0; i < (int)_children.size(); i++)
        delete _children[i];
}

