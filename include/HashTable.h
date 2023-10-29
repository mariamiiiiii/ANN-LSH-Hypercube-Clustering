#pragma once

#include <list>

#include "Image.h"
#include "HashFunctions.h"

using namespace std;


/* Class that keeps all the important information of a Hash Table, with all the required functions */
class Hash_Table {
    private:
        int w;                  // window
        int k;                  // num of LSH functions
        int dimension;          // num of LSH functions
        int buckets;            // buckets of hash table   

        list<Image*> *table;    // Array of buckets. Each bucket is a list of images
        Hash_Function *hash_function;
    public: 
        /* Constructor of Hash Table */
        Hash_Table(int k, int w, int dimension, int buckets); 
        
        /* Creates the g function */
        unsigned long g_function(Image *image);

        /* Return the bucket that corresponds this image image */
        list<Image*> getBucketImages(Image *image);

        /* Insert the image into the appropriate bucket */
        bool insert(Image *image);

        /* Destructor of Hash Table */
        ~Hash_Table();
}; 