
#pragma once

#include <float.h>
#include <iostream>
#include <fstream>
#include <map>

#include "../r64/ExqDescriptorR64.h"
#include "../IExqFunctions.h"

namespace exq {
    class ECPNearestNeighbour {
    public:
        ECPNearestNeighbour(ExqDescriptorR64* _query, uint32_t _k,
                            IExqFunctions<uint64_t>*& functions, int featureDimensions);

        ECPNearestNeighbour(uint32_t _k,
                            IExqFunctions<uint64_t>*& functions, int featureDimensions);

        ~ECPNearestNeighbour();

        // Get info
        ExqDescriptorR64* getQuery() { return query; }

        uint32_t getK() { return k; }

        // Find distance to a descriptor
        float Distance(ExqDescriptorR64* data);

        // The query processing interface is very simple:
        void compareAndReplaceFarthest(ExqDescriptorR64* data, uint32_t clusterId = -1);

        inline void PrintAnswer(int queryId) {
            cout << neighbors << " : " << neighbors << endl;
            for (uint64_t i = 0; i < neighbors; i++)
                cout << queryId << ":"
                    << i << ":"
                    << descriptorIDs[i] << ":"
                    << distances[i] << endl;
        }

        inline void open() {
            scanNext = 0;
        }

        inline uint32_t* nextClusterID() {
            if ((uint32_t)scanNext < neighbors) {
                return &(clusterIDs[scanNext]);
            }
            return NULL;
        }

        inline uint32_t* next() {
            if ((uint32_t)scanNext < neighbors) {
                return &(descriptorIDs[scanNext++]);
            }
            return NULL;
        }

        inline float* getDistanceForDescriptor() {
            return &(distances[scanNext-1]);
        }

        inline void close() {
            scanNext = -1;
        }

    private:
        // Query information
        ExqDescriptorR64* query;
        //int mod;
        uint32_t numdims{};
        IExqFunctions<uint64_t>* _functions;
        int _featureDimensions;

        // Query answer information
        uint32_t k;
        uint32_t neighbors;
        float *distances;
        uint32_t *descriptorIDs;
        uint32_t *clusterIDs;

        vector<float> extendedValueArr;
        vector<vector<int>> indexQueryArr;
        // Support for efficient computation
        // Index into the arrays where the farthest descriptor is found
        uint32_t farthest;

        // Replace the farthest neighbor info with this info
        inline void ReplaceFarthest(uint32_t id, uint32_t clusterId, float dist) {
            descriptorIDs[farthest] = id;
            clusterIDs[farthest]    = clusterId;
            distances[farthest]     = dist;
        }

        // Find the new farthest neigbor after replacing the old one
        void FindFarthest();

        void setHelperArrayForDistance();

        int scanNext{};

    };
}