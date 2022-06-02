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
        PyObject* videoIdsPy = PyList_GetItem(filtersPy,1);
        PyObject* stdFiltersPy = PyList_GetItem(filtersPy,2);
        PyObject* collFiltersPy = PyList_GetItem(filtersPy,3);
        PyObject* vidFiltersPy = PyList_GetItem(filtersPy,4);
        PyObject* negCollectionIdsPy = PyList_GetItem(filtersPy,5);
        PyObject* negVideoIdsPy = PyList_GetItem(filtersPy,6);
        PyObject* negStdFiltersPy = PyList_GetItem(filtersPy,7);
        PyObject* negCollFiltersPy = PyList_GetItem(filtersPy,8);
        PyObject* negVidFiltersPy = PyList_GetItem(filtersPy,9);
        PyObject* rangeFiltersPy = PyList_GetItem(filtersPy,10);
        cout << "Extracting Coll Id" << endl;
        // Collection and Video Id filters
        for (int i = 0; i < (int)PyList_Size(collectionIdsPy); i++) {
            filters.collection.insert((uint16_t) PyLong_AsLong(PyList_GetItem(collectionIdsPy,i)));
        }
        cout << "Extracting Vid Id" << endl;
        for (int i = 0; i < (int)PyList_Size(videoIdsPy); i++) {
            filters.video.insert((uint16_t) PyLong_AsLong(PyList_GetItem(videoIdsPy,i)));
        }
        cout << "Extracting Neg Coll Id" << endl;
        for (int i = 0; i < (int)PyList_Size(negCollectionIdsPy); i++) {
            filters.negCollection.insert((uint16_t) PyLong_AsLong(PyList_GetItem(collectionIdsPy,i)));
        }
        cout << "Extracting Neg Vid Id" << endl;
        for (int i = 0; i < (int)PyList_Size(negVideoIdsPy); i++) {
            filters.negVideo.insert((uint16_t) PyLong_AsLong(PyList_GetItem(videoIdsPy,i)));
        }

        cout << "Extracting Std" << endl;
        // Std Filters
        for (int i = 0; i < (int)PyList_Size(stdFiltersPy); i++) { // properties
            int size = (int) PyList_Size(PyList_GetItem(stdFiltersPy, i));
            if (size > 0) {
                PyObject *tmp = PyList_GetItem(stdFiltersPy, i);
                auto e = make_pair(i, set<uint16_t>());
                filters.stdFilters.push_back(e);
                int pos = (int) filters.stdFilters.size() - 1;
                for (int j = 0; j < size; j++) { // values
                    filters.stdFilters[pos].second.insert((uint16_t) PyLong_AsLong(PyList_GetItem(tmp, j)));
                }
            }
        }
        cout << "Extracting Neg Std" << endl;
        // Neg Std Filters
        for (int i = 0; i < (int)PyList_Size(negStdFiltersPy); i++) { // properties
            int size = (int)PyList_Size(PyList_GetItem(negStdFiltersPy,i));
            if (size > 0) {
                PyObject* tmp = PyList_GetItem(negStdFiltersPy,i);
                auto e = make_pair(i,set<uint16_t>());
                filters.negStdFilters.push_back(e);
                int pos = (int)filters.negStdFilters.size()-1;
                for (int j = 0; j < size; j++) { // values
                    filters.negStdFilters[pos].second.insert((uint16_t) PyLong_AsLong(PyList_GetItem(tmp,j)));
                }
            }
        }
        cout << "Extracting Collection" << endl;
        // Collection Filters
        for (int i = 0; i < (int)PyList_Size(collFiltersPy); i++) { // collections
            int cSize = (int)PyList_Size(PyList_GetItem(collFiltersPy,i));
            if (cSize > 0) {
                PyObject* tmpColl = PyList_GetItem(collFiltersPy,i);
                auto c = make_pair(i, vector<pair<int,set<uint16_t>>>());
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
                        auto e = make_pair(j,set<uint16_t>());
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
        cout << "Extracting Neg Coll" << endl;
        // Neg Collection Filters
        for (int i = 0; i < (int)PyList_Size(negCollFiltersPy); i++) { // collections
            int cSize = (int)PyList_Size(PyList_GetItem(negCollFiltersPy,i));
            if (cSize > 0) {
                PyObject* tmpColl = PyList_GetItem(negCollFiltersPy,i);
                auto c = make_pair(i, vector<pair<int,set<uint16_t>>>());
                filters.negCollFilters.push_back(c);
                int collPos = (int) filters.negCollFilters.size()-1;
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
                cout << "Init prop filters for collection " <<
                filters.collFilters[collPos].first << endl;
#endif
                for (int j = 0; j < cSize; j++) { // properties
                    int pSize = (int) PyList_Size(PyList_GetItem(tmpColl,j));
                    if (pSize > 0) {
                        PyObject* tmpProp = PyList_GetItem(tmpColl,j);
                        auto e = make_pair(j,set<uint16_t>());
                        filters.negCollFilters[collPos].second.push_back(e);
                        int pos = (int) filters.negCollFilters[collPos].second.size()-1;
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
                        cout << "Found " << pSize << " filters for property " <<
                        filters.collFilters[collPos].second[pos].first << endl;
#endif
                        for (int k = 0; k < pSize; k++) { // values
                            filters.negCollFilters[i].second[pos]
                                    .second.insert((uint16_t)PyLong_AsLong(PyList_GetItem(tmpProp,k)));
                        }
                    }
                }
            }
        }

        cout << "Extracting Vid" << endl;
        // Video filters
        for (int i = 0; i < (int)PyList_Size(vidFiltersPy); i++) { // collections
            int cSize = (int)PyList_Size(PyList_GetItem(vidFiltersPy,i));
            if (cSize > 0) {
                PyObject* tmpColl = PyList_GetItem(vidFiltersPy,i);
                auto c = make_pair(i, vector<pair<int,set<uint16_t>>>());
                filters.vidFilters.push_back(c);
                for (int j = 0; j < cSize; j++) { // properties
                    int pSize = (int) PyList_Size(PyList_GetItem(tmpColl,j));
                    if (pSize > 0) {
                        PyObject* tmpProp = PyList_GetItem(tmpColl,j);
                        auto e = make_pair(j,set<uint16_t>());
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
        cout << "Extracting Neg Vid" << endl;
        // Neg Video filters
        for (int i = 0; i < (int)PyList_Size(negVidFiltersPy); i++) { // collections
            int cSize = (int)PyList_Size(PyList_GetItem(negVidFiltersPy,i));
            if (cSize > 0) {
                PyObject* tmpColl = PyList_GetItem(negVidFiltersPy,i);
                auto c = make_pair(i, vector<pair<int,set<uint16_t>>>());
                filters.negVidFilters.push_back(c);
                for (int j = 0; j < cSize; j++) { // properties
                    int pSize = (int) PyList_Size(PyList_GetItem(tmpColl,j));
                    if (pSize > 0) {
                        PyObject* tmpProp = PyList_GetItem(tmpColl,j);
                        auto e = make_pair(j,set<uint16_t>());
                        filters.negVidFilters[i].second.push_back(e);
                        int pos = (int) filters.negVidFilters[i].second.size()-1;
                        for (int k = 0; k < pSize; k++) { // values
                            filters.negVidFilters[i].second[pos]
                                    .second.insert((uint16_t)PyLong_AsLong(PyList_GetItem(tmpProp,k)));
                        }
                    }
                }
            }
        }
        cout << "Extracting Range" << endl;
        // Range filters
        // Input: Coll[FilterProperty[FilterValues[ [key,op,v0,v1], ... ]]]
        for (int i = 0; i < (int)PyList_Size(rangeFiltersPy); i++) { // collections
            int cSize = (int)PyList_Size(PyList_GetItem(negVidFiltersPy,i));
            if (cSize > 0) {
                PyObject* tmpColl = PyList_GetItem(rangeFiltersPy,i);
                auto c = make_pair(i, vector<pair<int,map<uint16_t,array<uint16_t,3>>>>());
                filters.rangeFilters.push_back(c);
                for (int j = 0; j < cSize; j++) { // properties
                    int pSize = (int) PyList_Size(PyList_GetItem(tmpColl,j));
                    if (pSize > 0) {
                        PyObject* tmpProp = PyList_GetItem(tmpColl,j);
                        auto e = make_pair(j,map<uint16_t,array<uint16_t,3>>());
                        filters.rangeFilters[i].second.push_back(e);
                        int pos = (int) filters.rangeFilters[i].second.size()-1;
                        for (int k = 0; k < pSize; k++) { // values
                            auto key = (uint16_t) PyLong_AsLong(PyList_GetItem(PyList_GetItem(tmpProp,k),0));
                            auto op = (uint16_t) PyLong_AsLong(PyList_GetItem(PyList_GetItem(tmpProp,k),1));
                            auto v0 = (uint16_t) PyLong_AsLong(PyList_GetItem(PyList_GetItem(tmpProp,k),2));
                            auto v1 = (uint16_t) PyLong_AsLong(PyList_GetItem(PyList_GetItem(tmpProp,k),3));
                            filters.rangeFilters[i].second[pos].second[key] = array<uint16_t,3>();
                            filters.rangeFilters[i].second[pos].second[key][0] = op;
                            filters.rangeFilters[i].second[pos].second[key][1] = v0;
                            filters.rangeFilters[i].second[pos].second[key][2] = v1;
                        }
                    }
                }
            }
        }
    }
}

#endif //EXQUISITOR_EXQPYHELPERFUNCTIONS_H
