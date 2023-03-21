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

        vector<double> getTopDistances();

        vector<uint32_t> getTopIds();

        void printDescriptor(ExqDescriptorR64* data);
        void printStuff();

        // The query processing interface is very simple:
        void compareAndReplaceFarthest(ExqDescriptorR64* data, uint32_t clusterid = -1);

        // Find distance to a descriptor
        double distance(ExqDescriptorR64* data);

        void open();

        uint32_t* nextClusterID();  // NOTE: This does not advance the scan!
        uint32_t* next();

        void close();

        // Output the answer
        void PrintAnswer();
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


        // Replace the farthest neighbor info with this info
        void ReplaceFarthest(uint32_t id, uint32_t clusterid, double dist);

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