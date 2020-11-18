#pragma once

#include <cfloat>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "ExqDescriptor.h"
namespace exq {
    using std::vector;

    template<typename T, typename U, typename V>
    class ECPFarthestNeighbour {
    public:
        ECPFarthestNeighbour(vector<double>* query, double bias, uint64_t k);

        ~ECPFarthestNeighbour();

        // Get info
        vector<double> getQuery() { return *_query; }

        uint64_t getK() { return _k; }

        vector<double> getTopDistances();

        vector<uint32_t> getTopIds();

        void printDescriptor(ExqDescriptor<T,U,V>* data);
        void printStuff();

        // The query processing interface is very simple:
        void compareAndReplaceFarthest(ExqDescriptor<T,U,V>* data, uint64_t clusterid = -1);

        void open();

        uint64_t* nextClusterID();  // NOTE: This does not advance the scan!
        uint64_t* next();

        void close();

        // Output the answer
        void PrintAnswer();
    private:
        // Query information
        vector<double>* _query;
        double _bias;

        // Query answer information
        uint64_t _k;
        uint64_t _neighbors;
        vector<double> _distances;
        vector<uint64_t> _descriptorIDs;
        vector<uint64_t> _clusterIDs;

        // Support for efficient computation
        // Index into the arrays where the farthest descriptor is found
        uint64_t _farthest;

        // Find distance to a descriptor
        double Distance(ExqDescriptor<T,U,V>* data);

        // Replace the farthest neighbor info with this info
        void ReplaceFarthest(uint64_t id, uint64_t clusterid, double dist);

        // Find the new farthest neigbor after replacing the old one
        void FindFarthest();

        //Find length of vector
        double len();

        //Calculate cross product of two vectors of num_dim dimensions
        void CrossProduct(unsigned char *v1, unsigned char *v2, unsigned char *res);

        int _scanNext;
        int _countZeros;
        int _countIds;
    };
}