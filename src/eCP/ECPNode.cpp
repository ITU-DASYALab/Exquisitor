#include "ECPNode.h"
#include <cmath>

using namespace exq;

template<typename T, typename U, typename V>
ECPNode<T,U,V>::ECPNode(ExqDescriptor<T,U,V>* _centroid, bool _addChild)
{
    // Note the centroid
    centroid = _centroid;

    // Allocate space for children
    maxChildren = 200;
    numChildren = 0;
    children = vector<ExqDescriptor<T,U,V>*>(maxChildren);

    if (_addChild)
        addChild(new ExqDescriptor<T,U,V>(_centroid));
};

template<typename T, typename U, typename V>
ECPNode<T,U,V>::~ECPNode() {
    delete centroid;
    for (int i=0; i<numChildren; i++)
        delete children[i];
    delete children;
}

template<typename T, typename U, typename V>
void ECPNode<T,U,V>::addChild(ExqDescriptor<T,U,V>* child)
{
    // Handle overflows if the node is already full
    if (numChildren == maxChildren) {
        // Double the space and allocate a new vector
        maxChildren *= 2;
        auto tmpChildren = vector<ExqDescriptor<T,U,V>*>(maxChildren);

        // Copy the old data to the new vector
        for (int i=0; i<numChildren; i++) {
            tmpChildren[i] = children[i];
        }
        // Replace the old data with the new vector
        delete[] children;
        children = tmpChildren;
    };

    // Now there is enough space, so insert
    children[numChildren++] = child;
};

template<typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ECPNode<T,U,V>::get(uint64_t i)
{
    if (i > (uint64_t) numChildren) {
        return NULL;
    }
    return children[i];
}

template<typename T, typename U, typename V>
void ECPNode<T,U,V>::open()
{
    currChild = 0;
}

template<typename T, typename U, typename V>
ExqDescriptor<T,U,V>* ECPNode<T,U,V>::next()
{
    if (currChild < numChildren) {
        return children[currChild++];
    }
    return NULL;
}

template<typename T, typename U, typename V>
void ECPNode<T,U,V>::close()
{
    currChild = -1;
}

template<typename T, typename U, typename V>
void ECPNode<T,U,V>::PrintNode(string indent)
{
    cout << indent << "NODE: Centroid: ";
    PrintDescriptor(centroid);

    cout << indent << "NODE: numChildren = " << numChildren << " (maxChildren = " << maxChildren << ")" << endl;
    for (int i=0; i<numChildren; i++) {
        cout << indent << "NODE: Child " << i << ": ";
        PrintDescriptor(children[i]);
    }
}
