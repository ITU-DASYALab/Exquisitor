#include "ECPNode.h"
#include <cmath>

using namespace exq;

template<typename T, typename U, typename V>
ECPNode<T,U,V>::ECPNode(ExqDescriptor<T,U,V>* centroid, bool _addChild) {
    // Note the centroid
    _centroid = centroid;

    // Allocate space for children
    _maxChildren = 200;
    _children = vector<ExqDescriptor<T,U,V>*>();
    _children.reserve(_maxChildren);

    if (_addChild)
        addChild(new ExqDescriptor<T,U,V>(_centroid));
};

template<typename T, typename U, typename V>
ECPNode<T,U,V>::~ECPNode() {
    delete _centroid;
    for (int i = 0; i < (int)_children.size(); i++)
        delete _children[i];
}

template<typename T, typename U, typename V>
inline void ECPNode<T,U,V>::addChild(ExqDescriptor<T,U,V>* child) {
    // Now there is enough space, so insert
    _children.push_back(child);
};

template<typename T, typename U, typename V>
inline ExqDescriptor<T,U,V>* ECPNode<T,U,V>::get(uint32_t i) {
    if (i > _children.size()) {
        return NULL;
    }
    return _children[i];
}

template<typename T, typename U, typename V>
inline void ECPNode<T,U,V>::open() {
    _currChild = 0;
}

template<typename T, typename U, typename V>
inline ExqDescriptor<T,U,V>* ECPNode<T,U,V>::next() {
    if (_currChild < (int)_children.size()) {
        return _children[_currChild++];
    }
    return NULL;
}

template<typename T, typename U, typename V>
inline void ECPNode<T,U,V>::close() {
    _currChild = -1;
}

template<typename T, typename U, typename V>
inline void ECPNode<T,U,V>::PrintNode(string indent) {
    cout << indent << "NODE: Centroid: ";
    //PrintDescriptor(centroid);

    cout << indent << "NODE: numChildren = " << _children.size() << " (maxChildren = " << _maxChildren << ")" << endl;
    for (int i = 0; i < (int)_children.size(); i++) {
        cout << indent << "NODE: Child " << i << ": ";
        //PrintDescriptor(children[i]);
    }
}

template class exq::ECPNode<uint64_t, uint64_t, uint64_t>;

