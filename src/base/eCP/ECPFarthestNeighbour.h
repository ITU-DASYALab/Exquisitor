#pragma once

#include <cfloat>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "../r64/ExqDescriptorR64.h"
#include "../IExqFunctions.h"

namespace exq {
    using std::vector;

    class ECPFarthestNeighbour {
    public:
        ECPFarthestNeighbour(vector<double>& query, double bias, uint32_t k,
                             IExqFunctions<uint64_t>*& functions);

        ~ECPFarthestNeighbour();

        // Get info
        vector<double> getQuery() { return _query; }

        uint32_t getK() { return _k; }
        void updateK(uint32_t k);

        // The query processing interface is very simple:
        void compareAndReplaceFarthest(ExqDescriptorR64* data, uint32_t clusterid = -1);

        void printDescriptor(ExqDescriptorR64* data);

        inline uint32_t* nextClusterID() {
            if ((uint32_t)_scanNext < _neighbors) {
                return &(_clusterIDs[_scanNext]);
            }
            return NULL;
        }

        /* This is used for finding top clusters*/
        inline uint32_t* next() {
            if ((uint32_t)_scanNext < _neighbors) {
                return &(_descriptorIDs[_scanNext++]);
            }
            return NULL;
        };

        inline void open() {
            _scanNext = 0;
        }

        inline void close() {
            _scanNext = -1;
        }

        inline void printStuff() {
            cout << "count_ids is: " << _countIds << endl;
            cout << "cound_zeros is: " << _countZeros << endl;
        }

        // Find distance to a descriptor
        // simplified projection calculations from query vector (weight vector from SVM) and data vector
        inline double distance(ExqDescriptorR64* data) {
            return _functions->distance(_query, _bias, *data);
        };

        // Output the answer
        inline void PrintAnswer() {
            cout << _neighbors << " : " << _neighbors << endl;
            for (uint64_t i = 0; i < _neighbors; i++) {
                cout << i << ":" << _descriptorIDs[i] << ":" << _distances[i] << endl;
            }
        }

        inline vector<double> getTopDistances() {
            return _distances;
        }

        inline vector<uint32_t> getTopIds() {
            return _descriptorIDs;
        }
    private:
        // Query information
        vector<double> _query;
        double _bias;
        IExqFunctions<uint64_t>* _functions;

        // Query answer information
        uint32_t _k;
        uint32_t _neighbors;
        vector<double> _distances;
        vector<uint32_t> _descriptorIDs;
        vector<uint32_t> _clusterIDs;

        // Support for efficient computation
        // Index into the arrays where the farthest descriptor is found
        uint32_t _farthest;

        int _scanNext;
        int _countZeros;
        int _countIds;

        // Find the new farthest neigbor after replacing the old one
        void FindFarthest();

        // Replace the farthest neighbor info with this info
        inline void ReplaceFarthest(uint32_t id, uint32_t clusterid, double dist) {
            _descriptorIDs[_farthest] = id;
            _clusterIDs[_farthest]    = clusterid;
            _distances[_farthest]     = dist;
        }
    };
}