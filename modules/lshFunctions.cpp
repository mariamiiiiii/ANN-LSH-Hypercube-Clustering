#include "../include/lshFunctions.h"
#include <algorithm>
#include <unordered_set> 
#include <iostream>


/* Constructor of LSH */
LSH::LSH(int k, int w, int L, Dataset *image_dataset) {
    this->w = w;
    this->k = k;
    this->L = L;

    /* Take the images of the given Dataset */
    this->images = image_dataset->getImages();

    /* Create L Hash Tables */
    this->hashTables = new Hash_Table*[L];

    for (int i = 0; i < L; i++) {
        int dimension = image_dataset->getImageSize();
        int buckets = this->images.size() / TableSize;

        this->hashTables[i] = new Hash_Table(k, w, dimension, buckets);
    }

    /* Insert all images to Hash Tables */
    for (unsigned int i = 0; i < this->images.size(); i++) {
        for (int j = 0; j < L; j++) {
            this->hashTables[j]->insert(this->images[i]);
        }
    }
}


/* Finds approximate k nearest neighbors (kNN) of a given query image */
vector<pair<double, int>> LSH::approximate_kNN(Image* query, unsigned int N) {
    /* Store the distances and IDs of nearest neighbors */
    vector<pair<double, int>> neighbors;

    /* Keeps track of visited image IDs */
    unordered_set<int> visited;

    /* Iterate through each hash table */
    for (int i = 0; i < L; i++) {
        /* Rtrieves images that hash to the same bucket as the query image */
        list<Image*> bucket = this->hashTables[i]->getBucketImages(query);

        /* Iterate through the bucket */  
        for (Image* p : bucket) {
            /* Check if the image ID is not in the visited set */
            if (visited.find(p->getImageId()) == visited.end()) {
                /* Insert the image ID into the visited set */
                visited.insert(p->getImageId());

                /* Calculate distance between the query and current image with the given Metric */
                double distance = query->distance(p, Metric); 

                /* Create a pair of distance and image ID and add it to neighbors */
                pair<double, int> tmp(distance, p->getImageId());
                neighbors.push_back(tmp);
            }
        }
    }

    /* Sorts the neighbor based on the distances of the nearest neighbors */
    sort(neighbors.begin(), neighbors.end());

    /* Ensures that the vector contains at most N nearest neighbors */
    if (neighbors.size() > N) {
        neighbors.resize(N);
    }

    return neighbors;
}


/* Finds all images within a given range */
vector<Image*> LSH::rangeSearch(Image *query, double range) {
    /* Store the images within the given range */
    vector<Image*> neighbors;
    /* Keeps track of visited image IDs */
    unordered_set<int> visited;

    /* Iterates through each Hash Table L */
    for (int i = 0; i < L; i++) {
        /* Retrieves a bucket of images */
        list<Image*> bucket = this->hashTables[i]->getBucketImages(query);

        /* For each image in the bucket */
        for (Image* image : bucket) {
            /* Checks if the image is not visited */
            if (visited.find(image->getImageId()) == visited.end()) {
                if (query->distance(image, Metric) <= range) {
                    visited.insert(image->getImageId());
                    neighbors.push_back(image);
                }
            }
        }
    }

    return neighbors;
}


/* Destructor of LSH */
LSH::~LSH() {
    // Destroy all hash tables
    for (int i = 0; i < this->L; i++) {
        delete this->hashTables[i];
    }
    delete[] this->hashTables;
}