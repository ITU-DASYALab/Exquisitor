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

inline void ECPNode::addChild(ExqDescriptorR64* child) {
    // Now there is enough space, so insert
    _children.push_back(child);
};

inline ExqDescriptorR64* ECPNode::get(uint32_t i) {
    if (i > _children.size()) {
        return NULL;
    }
    return _children[i];
}

inline void ECPNode::open() {
    _currChild = 0;
}

inline ExqDescriptorR64* ECPNode::next() {
    if (_currChild < (int)_children.size()) {
        return _children[_currChild++];
    }
    return NULL;
}

inline void ECPNode::close() {
    _currChild = -1;
}

inline void ECPNode::PrintNode(string indent) {
    cout << indent << "NODE: Centroid: ";
    //PrintDescriptor(centroid);

    cout << indent << "NODE: numChildren = " << _children.size() << " (maxChildren = " << _maxChildren << ")" << endl;
    for (int i = 0; i < (int)_children.size(); i++) {
        cout << indent << "NODE: Child " << i << ": ";
        //PrintDescriptor(children[i]);
    }
}