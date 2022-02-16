//
// Created by Omar Shahbaz Khan on 15/02/2022.
//

#ifndef EXQUISITOR_EXQPYHELPERFUNCTIONS_H
#define EXQUISITOR_EXQPYHELPERFUNCTIONS_H

#include "Metadata/ItemFilter.h"

using namespace exq;

namespace exq {
    using std::make_pair;

    void extractFiltersFromPythonObject(PyObject* filtersPy, Filters& filters) {
        PyObject* collectionIdsPy = PyList_GetItem(filtersPy,0);
        PyObject* stdFiltersPy = PyList_GetItem(filtersPy,1);
        PyObject* collFiltersPy = PyList_GetItem(filtersPy,2);
        PyObject* vidFiltersPy = PyList_GetItem(filtersPy,3);

        for (int i = 0; i < (int)PyList_Size(collectionIdsPy); i++) {
            filters.collection.insert((uint16_t) PyLong_AsLong(PyList_GetItem(collectionIdsPy,i)));
        }
        for (int i = 0; i < (int)PyList_Size(stdFiltersPy); i++) { // properties
            int size = (int)PyList_Size(PyList_GetItem(stdFiltersPy,i));
            if (size > 0) {
                PyObject* tmp = PyList_GetItem(stdFiltersPy,i);
                pair<int,set<uint16_t>> e = make_pair(i,set<uint16_t>());
                filters.stdFilters.push_back(e);
                int pos = (int)filters.stdFilters.size()-1;
                for (int j = 0; j < size; j++) { // values
                    filters.stdFilters[pos].second.insert((uint16_t) PyLong_AsLong(PyList_GetItem(tmp,j)));
                }
            }
        }
        for (int i = 0; i < (int)PyList_Size(collFiltersPy); i++) { // collections
            int cSize = (int)PyList_Size(PyList_GetItem(collFiltersPy,i));
            if (cSize > 0) {
                PyObject* tmpColl = PyList_GetItem(collFiltersPy,i);
                pair<int,vector<pair<int,set<uint16_t>>>> c = make_pair(i, vector<pair<int,set<uint16_t>>>());
                filters.collFilters.push_back(c);
                int collPos = (int) filters.collFilters.size()-1;
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
                cout << "Init prop filters for collection " <<
                filters.collFilters[collPos].first << endl;
#endif
                for (int j = 0; j < cSize; j++) { // properties
                    int pSize = (int) PyList_Size(PyList_GetItem(tmpColl,j));
                    if (pSize > 0) {
                        PyObject* tmpProp = PyList_GetItem(tmpColl,j);
                        pair<int,set<uint16_t>> e = make_pair(j,set<uint16_t>());
                        filters.collFilters[collPos].second.push_back(e);
                        int pos = (int) filters.collFilters[collPos].second.size()-1;
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
                        cout << "Found " << pSize << " filters for property " <<
                        filters.collFilters[collPos].second[pos].first << endl;
#endif
                        for (int k = 0; k < pSize; k++) { // values
                            filters.collFilters[i].second[pos]
                            .second.insert((uint16_t)PyLong_AsLong(PyList_GetItem(tmpProp,k)));
                        }
                    }
                }
            }
        }
        for (int i = 0; i < (int)PyList_Size(vidFiltersPy); i++) { // collections
            int cSize = (int)PyList_Size(PyList_GetItem(vidFiltersPy,i));
            if (cSize > 0) {
                PyObject* tmpColl = PyList_GetItem(vidFiltersPy,i);
                pair<int,vector<pair<int,set<uint16_t>>>> c = make_pair(i, vector<pair<int,set<uint16_t>>>());
                filters.vidFilters.push_back(c);
                for (int j = 0; j < cSize; j++) { // properties
                    int pSize = (int) PyList_Size(PyList_GetItem(tmpColl,j));
                    if (pSize > 0) {
                        PyObject* tmpProp = PyList_GetItem(tmpColl,j);
                        pair<int,set<uint16_t>> e = make_pair(j,set<uint16_t>());
                        filters.vidFilters[i].second.push_back(e);
                        int pos = (int) filters.vidFilters[i].second.size()-1;
                        for (int k = 0; k < pSize; k++) { // values
                            filters.vidFilters[i].second[pos]
                                    .second.insert((uint16_t)PyLong_AsLong(PyList_GetItem(tmpProp,k)));
                        }
                    }
                }
            }
        }
    }
}

#endif //EXQUISITOR_EXQPYHELPERFUNCTIONS_H
