#pragma once


#include <unordered_map>
#include <list>
#include "image.h"
#include "../include/HashFunctions.h"
#include "../include/Dataset.h"

using namespace std;

/* Metric (2 for Euclidean) */
#define Metric 2

/* Class that keeps all the important information of Hypercube, with all the required functions */
class Hypercube {
    private:
        int d; 
        int w;              // window
        int dimension;      // dimension of images
        
        Hash_Function *h;
        unordered_map<unsigned long,int> *f;
        list<Image*> *vertices;
    public:
        /* Constructor of Hypercube */
        Hypercube(Dataset *dataset, int dimension, int w);

        /* Implementing h Hash Function */
        unsigned int h_function(Image *image);

        /* Insert image from the dataset to the hypercube */
        bool insert(Image *image);

        /* Performs a recursive search */
        list<Image*> search(unsigned int M, int probes, int curVertex, int checked_mask, double R, Image *q);

        /* Performs a search for similar points within a hypercube */
        list<pair<double, int>> searchSimilarPoints(Image *image, int M, int probes);

        /* Performs Range Search */
        list<Image*> rangeSearch(Image *image, int M, int probes, double R);

        /* Function to Calculate Power */
        unsigned long power(int base, int exponent);

        /* Destructor of Hypercube */
        ~Hypercube();
};