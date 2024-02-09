#include "ECPIndex.h"

#include <utility>

using std::ofstream;
using std::ios_base;
using std::unordered_set;

using namespace exq;

ECPIndex::ECPIndex(ECPConfig *cnfg, IExqFunctions<uint64_t>*& func, int featureDimensions,
                   int modality, vector<ExqDescriptorR64*>* descs, vector<ItemProperties> itemProps, vector<vector<Props>> vidProps,
                   ExpansionType expansionType, int statLevel) {
    _cnfg = cnfg;
    cout << "Descriptor size: " << func->getDescriptorSize() << endl;
    _indexEntrySize = (sizeof(int) * 3) + sizeof(uint32_t) + func->getDescriptorSize();
    cout << "Index Entry Size: " << _indexEntrySize << endl;
    // Open the indx file
    string indxPath = cnfg->getPathIdx();
    _indxFile = fopen(indxPath.c_str(), "rb");
    if (_indxFile == nullptr) {
        cout << "Index: Could not open index file " << indxPath << endl;
        exit(EXIT_FAILURE);
    }

    // Open the data file, needed for the creation of the clusters
    string dataPath = cnfg->getPathDat();
    _dataFile = fopen(dataPath.c_str(), "rb");
    if (_dataFile == nullptr) {
        cout << "Index: Could not open data file " << dataPath << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Index files loaded" << endl;
    // Double check the file size
    struct stat filestat{};
#ifdef _MSC_VER
    fstat(_fileno(_indxFile), &filestat);
#else
    fstat(fileno(_indxFile), &filestat);
#endif

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

    // Set metadata
    _itemProperties = std::move(itemProps);
    _vidProperties = std::move(vidProps);

    cout << "(ECPIndx) Initializing index data structure" << endl;
    // Initialize the data structure for the index file
    vector<ExqDescriptorR64*> centroids = vector<ExqDescriptorR64*>(_maxClusters);
    _clusters.resize(_maxClusters);

    cout << "(ECPIndx) Reading cluster info and centroids" << endl;
    // Read the cluster info and centroids from the indx file
    for (uint32_t i = 0; i < _maxClusters; i++) {
        _clusters[i] = new ECPCluster(cnfg, _indxFile, _dataFile, _indexEntrySize, func->getIota());
        _totalItems += _clusters[i]->getNumDescriptors();
        centroids[i] = new ExqDescriptorR64(_indxFile);
#if defined(DEBUG) || defined(DEBUG_INIT)
        cout << "(ECPIndx) Cluster " << i << " loaded" << endl;
#endif
    }

    cout << "(ECPIndx) Resize descriptor list" << endl;
    _descs = descs;
    _descs->resize(_totalItems);
    
    cout << "(ECPIndx) Initializing QOP object" << endl;
    _qop = new ECPQueryOptimisationPolicies(expansionType, statLevel, _clusters,
                                            &_itemProperties, &_vidProperties, modality);

    cout << "Creating Tree object" << endl;
    // Create the tree from the cluster information
    this->_tree = new ECPTree(cnfg, centroids, _maxClusters, func, featureDimensions, _qop, _descs);

    for (uint32_t i = 0; i < _maxClusters; i++) {
        delete centroids[i];
    }
}

ECPIndex::~ECPIndex() {
    // Close the files
    fclose(_dataFile);
    fclose(_indxFile);

    // Clean up memory
    delete _tree;
    delete _descs;
    for (uint32_t i = 0; i < _maxClusters; i++) {
        delete _clusters[i];
    }
}

void ECPIndex::loadDescriptors() {
    int indx = 0;

#if defined(DEBUG) || defined(DEBUG_INIT)
    cout << "(ECPIndx) Loading descriptors" << endl;
#endif

#if defined(DEBUG) || defined(DEBUG_INIT)
    printf("(ECPIndx) maxClusters: %u\n", _maxClusters);
#endif
    for (uint32_t i = 0; i < _maxClusters; i++) {
#if defined(DEBUG) || defined(DEBUG_INIT)
        printf("(ECPIndx) Open cluster %u\n", i);
#endif
        this->_clusters[i]->open();
        ExqDescriptorR64* descriptor;

        while ((descriptor = _clusters[i]->next()) != nullptr) {
#if defined(DEBUG) || defined(DEBUG_INIT)
            printf("(ECPIndx) Cluster %u indx %d descriptor id %u\n", i, indx, descriptor->id);
#endif
            uint32_t id = descriptor->getId();
            _clusters[i]->setDescriptorId(indx, id);
            _descs->at(id) = new ExqDescriptorR64(descriptor);
            delete descriptor;
            indx++;
        }
        indx = 0;
    }

    //cout << "Checking if cluster and desc offset match" << endl;
    //uint32_t offset = _cnfg->getNumOfst();
    //for (int i = 0; i < 10; i++) {
    //    if (_clusters[i]->getNumDescriptors() > 0)
    //        cout << "Desc " << _descs->at(offset*i)->id << " | Clst id is " << _clusters[i]->descriptorIds[0] << endl;
    //    else
    //        cout << "Desc " << _descs->at(offset*i)->id << " | Clst has no children" << endl;
    //}
}

bool ECPIndex::set_b_clusters(vector<float> query, float bias, int b, bool resume) {
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
    cout << "(ECPIndx) Searching for farthest neighbor" << endl;
#endif
    ECPFarthestNeighbour* clusters;
    if (!resume) {
        clusters = _tree->search(query, bias, b, _clusters);
    } else {
        clusters = _tree->search_pq(query, bias, b, _clusters);
    }
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
    cout << "(ECPIndx) Got clusters" << endl;
    cout << "descriptorIds size (" << clusters->getTopIds().size()  << ")" << endl;
#endif
    _bClusters.clear();
    clusters->open();
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
    cout << "(ECPIndx) Adding to _bClusters (size: " << _bClusters.size() << ")" << endl;
#endif
    uint32_t* clusterId;
    while((clusterId = clusters->next()) != nullptr) {
        _bClusters.push_back(*clusterId);
    }
#if defined(DEBUG) || defined(DEBUG_TRAIN) || defined(DEBUG_SUGGEST)
    cout << "(ECPIndx) Index Ready (freeing & returning)... _bClusters (size: " << _bClusters.size() << ")" << endl;
#endif
    delete clusters;
    return _tree->check_pq();
}


void ECPIndex::search(int chnk, int& totalData, vector<uint32_t>& suggIds,
                      int run, int segments, unordered_set<uint32_t>& seenItems, ItemFilter& filters) {
    int start = chnk * run;
    int end = start + chnk;
    int numDesc;
    totalData = 0;
    for (int cnt = start; cnt < end; cnt++) {
        numDesc = _clusters[_bClusters[cnt]]->getNumDescriptors();
        totalData += numDesc;
    }
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ECPIndx) Segment " << run << ", Total data: " << totalData << endl;
#endif
    suggIds = vector<uint32_t>(totalData);
    int j = 0;
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << "(ECPIndx) Segment " << run << " suggestions: ";
#endif
    for (int cnt = start; cnt < end; cnt++) {
        numDesc = _clusters[_bClusters[cnt]]->getNumDescriptors();
        for (int i = 0; i < numDesc; i++) {
            int passed = 0;
            //cout << "Check item " << _clusters[_bClusters[cnt]]->descriptorIds[i] << endl;
            if (!seenItems.contains(_clusters[_bClusters[cnt]]->descriptorIds[i])
                && filters.compare(_itemProperties[_clusters[_bClusters[cnt]]->descriptorIds[i]], _vidProperties)) {
                //cout << "passed" << endl;
                suggIds[j] = _clusters[_bClusters[cnt]]->descriptorIds[i];
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
                cout << suggIds[j] << ", ";
#endif
                passed++;
                j++;
            }

            if (_qop->getExpType() == ExpansionType::FILTER_REMAINING_CNT ||
                _qop->getExpType() == ExpansionType::ALL_REMAINING_CNT) {
                if (passed <= _qop->getFilterRemainingCount(_bClusters[cnt])) {
                    _qop->setFilterRemainingCount(_bClusters[cnt], passed);
                }
            }
        }
    }
#if defined(DEBUG) || defined(DEBUG_SUGGEST)
    cout << endl;
#endif
    suggIds.resize(j);
}

void ECPIndex::saveClusterDistribution(uint64_t numC, ECPConfig* cnfg, FILE* indxFile, FILE* dataFile, int mod) {
    /*CLUSTER DISTRIBUTION*/
    ofstream myfile;
    myfile.open("clusters.txt", ios_base::app);
    cout << "writing cluster info to file in mod: " << mod << endl;
    // Initialize the data structure for the index file
    vector<ExqDescriptorR64*> centroids = vector<ExqDescriptorR64*>(numC);
    this->_clusters = vector<ECPCluster*>(numC);

    // Read the cluster info and centroids from the indx file
    for (uint64_t i = 0; i < numC; i++) {
        _clusters[i] = new ECPCluster(cnfg, indxFile, dataFile, _indexEntrySize, 1);
        myfile << "BCP\t" << mod << "\t" << i << "\t" << "Number-of-Descriptors" << "\t"
               << _clusters[i]->getNumDescriptors() << endl;
        centroids[i] = new ExqDescriptorR64(indxFile);
    }
    myfile.close();
}

void ECPIndex::updateSessionInfo(vector<uint32_t> suggs) {
    _qop->updateSessionClusterCount(suggs);
}

void ECPIndex::resetSessionInfo(int level) {
    if (level == 0) // Full reset
        _qop->resetSession();
    if (level == 1) // FRC reset
        _qop->resetFilterCount();
}