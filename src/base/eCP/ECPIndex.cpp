#include "ECPIndex.h"

using std::ofstream;
using std::ios_base;

using namespace exq;

template <typename T, typename U, typename V>
ECPIndex<T,U,V>::ECPIndex(ECPConfig *cnfg, ExqFunctions<ExqDescriptor<T,U,V>>*& func, int featureDimensions,
                          ExpansionType expansionType, int statLevel) {
    _cnfg = cnfg;
    _indexEntrySize = (sizeof(int) * 3) + sizeof(uint64_t) + func->getDescriptorSize();
    // Open the indx file
    string indxPath = cnfg->getPathIdx();
    _indxFile = fopen(indxPath.c_str(), "rb");
    if (_indxFile == NULL) {
        cout << "Index: Could not open index file " << indxPath << endl;
        exit(EXIT_FAILURE);
    }

    // Open the data file, needed for the creation of the clusters
    string dataPath = cnfg->getPathDat();
    _dataFile = fopen(dataPath.c_str(), "rb");
    if (_dataFile == NULL) {
        cout << "Index: Could not open data file " << dataPath << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Index files loaded" << endl;
    // Double check the file size
    struct stat filestat;
    fstat(fileno(_indxFile), &filestat);
    _maxClusters = filestat.st_size / _indexEntrySize;
    if (_maxClusters != cnfg->getNumClst()) {
        cout << "Index: Incorrect index file size " << filestat.st_size << \
        " (does not contain  " << _cnfg->getNumClst() << " clusters)" << endl;
        exit(EXIT_FAILURE);
    }
    if (filestat.st_size % _indexEntrySize != 0 ) {
        cout << "Index: Incorrect index file size " << filestat.st_size << \
        " (not a multiple of " << _indexEntrySize << ")" << endl;
        exit(EXIT_FAILURE);
    }

    // Initialize the data structure for the index file
    vector<ExqDescriptor<T,U,V>*> centroids = vector<ExqDescriptor<T,U,V>*>(_maxClusters);
    _clusters = vector<ECPCluster<T,U,V>*>(_maxClusters);

    // Read the cluster info and centroids from the indx file
    for (uint64_t i = 0; i < _maxClusters; i++) {
        _clusters[i] = new ECPCluster<T,U,V>(cnfg, _indxFile, _dataFile, _indexEntrySize);
        _totalItems += _clusters[i]->getNumDescriptors();
        centroids[i] = new ExqDescriptor<T,U,V>(_indxFile);
    }

    _qop = new ECPQueryOptimisationPolicies<T,U,V>(expansionType, statLevel, _clusters);

    cout << "Creating Tree object" << endl;
    // Create the tree from the cluster information
    this->_tree = new ECPTree(cnfg, centroids, _maxClusters, func, featureDimensions, _qop);


    for (uint64_t i = 0; i < _maxClusters; i++) {
        delete centroids[i];
    }
};

template <typename T, typename U, typename V>
ECPIndex<T,U,V>::~ECPIndex() {
    // Close the files
    fclose(_dataFile);
    fclose(_indxFile);

    // Clean up memory
    delete _tree;
    for (uint64_t i = 0; i < _maxClusters; i++) {
        delete _clusters[i];
    }
}

template <typename T, typename U, typename V>
void ECPIndex<T,U,V>::loadDescriptors(vector<ExqDescriptor<T,U,V>*>* descs) {
    int indx = 0;
    int maxClusterSize = 100000000; //can be adjusted, this was to get rid of the huge clusters that were actually full of crap

    *descs = vector<ExqDescriptor<T,U,V>*>(_totalItems);
    //printf("maxClusters: %u\n", maxClusters);
    for (uint64_t i = 0; i < _maxClusters; i++) {
        //printf("Open cluster %u\n", i);
        this->_clusters[i]->open();
        ExqDescriptor<T,U,V>* descriptor;
        //skip clusters that are larger than maxClusterSixe
        if (this->_clusters[i]->getNumDescriptors() > maxClusterSize) {
            //printf("Cluster %u skipped\n", i);
            continue;
        }
        while ((descriptor = _clusters[i]->next()) != NULL) {
            //printf("Cluster %u indx %d\n", i, indx);
            uint32_t id = descriptor->id;
            _clusters[i]->setDescriptorId(indx, id);
            *descs->at(id) = new ExqDescriptor<T,U,V>(descriptor);
            delete descriptor;
            indx++;
        }
        indx = 0;
    }
}

template <typename T, typename U, typename V>
void ECPIndex<T,U,V>::set_b_clusters(vector<double>& query, double bias, int b) {
    ECPFarthestNeighbour<T,U,V>* clusters = _tree->search(query, bias, b, _clusters);
    _bClusters.clear();
    clusters->open();
    uint64_t* clusterId;
    while((clusterId = clusters->next()) != NULL) {
        _bClusters.push_back(*clusterId);
    }
}

template <typename T, typename U, typename V>
void ECPIndex<T,U,V>::search(int chnk, int& totalData, vector<uint32_t>& suggIds, vector<uint32_t>& suggToCluster,
                             int run, int segments) {
    int start = chnk * run;
    int end = start + chnk;
    int numDesc = 0;
    totalData = 0;
    for (int cnt = start; cnt < end; cnt++) {
        numDesc = this->_clusters[_bClusters[cnt]]->getNumDescriptors();
        totalData += numDesc;
    }
    //printf("(%d) INDEX(%d) - totalData(%d): %d\n", workerId, mod, run, totalData);
    suggIds = vector<uint32_t>(totalData);
    suggToCluster = vector<uint32_t>(totalData);
    int j = 0;
    for (int cnt = start; cnt < end; cnt++) {
        numDesc = _clusters[_bClusters[cnt]]->getNumDescriptors();
        for (int i = 0; i < numDesc; i++) {
            //Descriptor* descriptor = clusters[*clusterID]->descriptorList[i];
            suggIds[j] = _clusters[_bClusters[cnt]]->descriptorIds[i];
            suggToCluster[j] = _bClusters[cnt];
            j++;
        }
    }
}

template <typename T, typename U, typename V>
void ECPIndex<T,U,V>::saveClusterDistribution(uint64_t numC, ECPConfig* cnfg, FILE* indxFile, FILE* dataFile, int mod) {
    /*CLUSTER DISTRIBUTION*/
    ofstream myfile;
    myfile.open("clusters.txt", ios_base::app);
    cout << "writing cluster info to file in mod: " << mod << endl;
    // Initialize the data structure for the index file
    vector<ExqDescriptor<T,U,V>*> centroids = vector<ExqDescriptor<T,U,V>*>(numC);
    this->_clusters = vector<ECPCluster<T,U,V>*>(numC);

    // Read the cluster info and centroids from the indx file
    for (uint64_t i = 0; i < numC; i++) {
        _clusters[i] = new ECPCluster<T,U,V>(cnfg, indxFile, dataFile, _indexEntrySize);
        myfile << "BCP\t" << mod << "\t" << i << "\t" << "Number-of-Descriptors" << "\t"
               << _clusters[i]->getNumDescriptors() << endl;
        centroids[i] = new ExqDescriptor<T,U,V>(indxFile);
    }
    myfile.close();
}

template class exq::ECPIndex<uint64_t, uint64_t, uint64_t>;
