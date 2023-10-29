#include <iostream>

#include "../include/HashTable.h"


/* Constructor of Hash Table */
Hash_Table::Hash_Table(int k, int w, int dimension, int buckets) {
    this->w = w;
    this->k = k;
    this->dimension = dimension;
    this->buckets = buckets;

    /* Create the Hash Functions */ 
    this->hash_function = new Hash_Function(k, w, dimension);

    /* Create the buckets of Hash Table */ 
    this->table = new list<Image*>[buckets]; 
}


/* Creates the g function */
unsigned long Hash_Table::g_function(Image *image) {
    unsigned long g = 0;

    /* For k(num of LSH Functions)*/
    for (int i = 0; i < this->k; i++) {
        /* haching the images */
        unsigned long hash_value = this->hash_function->h_function(image, i);
        g = g + (hash_value * hash_value);
    }

    return g;
}


/* Return the bucket that corresponds this image image */
list<Image*> Hash_Table::getBucketImages(Image* image) {
    return this->table[this->g_function(image) % this->buckets];
}


/* Insert the image into the appropriate bucket */
bool Hash_Table::insert(Image *image) {
    unsigned long hash_value = this->g_function(image);

    this->table[hash_value % this->buckets].push_back(image);
    
    return true;
}


/* Destructor of Hash Table */
Hash_Table::~Hash_Table() {
    for(int i = 0; i < buckets; i++)
        this->table[i].clear();

    delete[] this->table;
    delete this->hash_function;
}
