#pragma once

#include <vector>
#include "image.h"
#include "dataset.h"
#include "HashTable.h"

using namespace std;

/* Value that affects the size of the Hash Table (usually 4 or 8 or 16) */
#define TableSize 8
/* Metric (2 for Euclidean) */
#define Metric 2

/* Class that keeps all the important information of LSH, with all the required functions */
class LSH {
    private:
        int w;      // window
        int k;      // num of LSH functions
        int L;      // num of hash tables
        
        vector<Image*> images;
        Hash_Table **hashTables;
    public:
        /* Constructor of LSH */
        LSH(int k,int w,int L, Dataset *imageDataset);

        /* Finds approximate k nearest neighbors (kNN) of a given query image */
        vector<pair<double, int>> approximate_kNN(Image *query, unsigned int N);

        /* Finds all images within a given range */
        vector<Image*> rangeSearch(Image *query, double range);

        /* Destructor of LSH */
        ~LSH();
};