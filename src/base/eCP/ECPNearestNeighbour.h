#pragma once
#include <float.h>
#include <iostream>
#include <fstream>
#include <map>

#include "../ExqDescriptor.h"
#include "../ExqFunctions.h"

namespace exq {
    template<typename T, typename U, typename V>
    class ECPNearestNeighbour {
    public:
        ECPNearestNeighbour(ExqDescriptor<T,U,V>* _query, uint32_t _k,
                            ExqFunctions<ExqDescriptor<T,U,V>>*& functions, int featureDimensions);

        ECPNearestNeighbour(uint32_t _k,
                            ExqFunctions<ExqDescriptor<T,U,V>>*& functions, int featureDimensions);

        ~ECPNearestNeighbour();

        // Get info
        ExqDescriptor<T,U,V>* getQuery() { return query; }

        uint32_t getK() { return k; }

        // Find distance to a descriptor
        double Distance(ExqDescriptor<T,U,V> *data);

        // The query processing interface is very simple:
        void compareAndReplaceFarthest(ExqDescriptor<T,U,V>* data, uint32_t clusterId = -1);

        void open();

        uint32_t* nextClusterID();  // NOTE: This does not advance the scan!
        uint32_t* next();

        double* getDistanceForDescriptor();

        void close();

        // Output the answer
        void PrintAnswer(int queryid);

    private:
        // Query information
        ExqDescriptor<T,U,V>* query;
        //int mod;
        uint32_t numdims{};
        ExqFunctions<ExqDescriptor<T,U,V>>* _functions;
        int _featureDimensions;

        // Query answer information
        uint32_t k;
        uint32_t neighbors;
        double *distances;
        uint32_t *descriptorIDs;
        uint32_t *clusterIDs;

        vector<double> extendedValueArr;
        vector<vector<double>> indexQueryArr;
        // Support for efficient computation
        // Index into the arrays where the farthest descriptor is found
        uint32_t farthest;

        // Replace the farthest neighbor info with this info
        void ReplaceFarthest(uint32_t id, uint64_t clusterId, double dist);

        // Find the new farthest neigbor after replacing the old one
        void FindFarthest();

        //Find length of vector
        double len(unsigned char* v);

        //Calculate cross product of two vectors of num_dim dimensions
        void CrossProduct(unsigned char* v1, unsigned char* v2, unsigned char* res);

        void setHelperArrayForDistance();

        int scanNext{};


    };
}