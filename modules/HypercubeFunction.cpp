#include <random>
#include <cmath>
#include <iostream>

#include "../include/HypercubeFunction.h"

#define INF 1.0/0.0


/* Constructor of Hypercube */
Hypercube::Hypercube(Dataset *imageDataset, int dimension, int w) {
    this->w = w;
    this->dimension = dimension;
    this->d = imageDataset->getImageSize();


    vector<Image*> images = imageDataset->getImages();

    /* Initialize hash function */
    this->h = new Hash_Function(this->dimension, this->w, this->d);

    // Initialize hash maps for fi functions
    this->f = new unordered_map<unsigned long, int>[this->dimension];

    /* Initialize containers for projection points */
    this->vertices = new list<Image*>[power(Metric, this->dimension)];

    /* Insert all the images from the dataset to the hypercube */
    for(unsigned int i = 0; i < images.size(); i++) {
        this->insert(images[i]);
    }
}


/* Implementing h Hash Function */
unsigned int Hypercube::h_function(Image *image) {
    /* Initializes a random number generator engine */
    default_random_engine generator;
    /* Defines a uniform distribution for generating random integers between 0 and 1 */
    uniform_int_distribution<int> uniform_distribution(0, 1);


    unsigned int result = 0;

    /* Generate hash value (project vector to a vertice in the hypercube) */
    for (int i = 0; i < this->dimension; i++) {
        /* Computes a hash value for the specific dimension using the hyperplane hash function */
        unsigned long hash = this->h->h_function(image, this->dimension - 1 - i);

        /* Check if we have already generated a bit for the d - i th hash */
        if (this->f[this->dimension - 1 - i].find(hash) == this->f[this->dimension - 1 - i].end()) {
            /* If not, generate it now by flipping a coin */
            this->f[this->dimension - 1 - i][hash] = uniform_distribution(generator);
        }
        
        result = result | (this->f[this->dimension - 1 - i][hash] << i);
    }
    
    return result;
}

/* Insert image from the dataset to the hypercube */
bool Hypercube::insert(Image *image) {
    this->vertices[this->h_function(image)].push_back(image);
    return true;
}


/* Performs a recursive search */
list<Image*> Hypercube::search(unsigned int M, int probes, int cur_node, int checked_mask, double R,Image *image) {
    /* Base case */
    if (probes <= 0) {
        list<Image*> emptyList;
        return emptyList;
    }

    /* Search current node */
    list<Image*> result;

    for (Image* neighborImage : this->vertices[cur_node]) {
        // Check if current point distance to q lies in range R
        if (image->distance(neighborImage, Metric) <= R) {
            result.push_back(neighborImage);
        }
    }

    
    if (result.size() > M) {
        result.resize(M);
    }
    M -= result.size();
    probes--;

    /* Recursively search non searched neighbour vertices (hamming) */
    for (int i = 0; M > 0 && probes > 0 && i < this->dimension; i++) {
        /* Check if ith bit was not previously hammed */
        if (!(checked_mask & (1 << i))) {
            list<Image*> rec = search(M, probes - this->dimension, cur_node ^ (1 << i), checked_mask | (1 << i), R, image);
            M -= rec.size();
            probes--;
            result.splice(result.end(), rec);
        }
    }
    
    return result;
}


/* Performs a search for similar points within a hypercube */
list<pair<double,int>> Hypercube::searchSimilarPoints(Image *image,int M, int probes) {
    list<Image*> ret = this->search(M,probes,this->h_function(image),0, INF,image);
    list<pair<double,int>> result;

    for (Image* similarImage : ret) {
        double distance = similarImage->distance(image, Metric);
        int imageId = similarImage->getImageId();
        result.emplace_back(distance, imageId);
    }
    
    return result;
}


/* Performs Range Search */
list<Image*> Hypercube::rangeSearch(Image *image, int M, int probes, double R) {
    return this->search(M, probes, this->h_function(image), 0, R, image);
}


/* Function to Calculate Power */
unsigned long Hypercube::power(int base, int exponent) {
    unsigned long result = 1;

    while (exponent > 0) {
        /* Check if exponent is odd */
        if (exponent % 2 == 1) {
            result = result * base;
        }

        /* square the base */
        base = base * base;

        /* divide the exponent by 2 */
        exponent =  exponent / 2;
    }

    return result;
}


/* Destructor of Hypercube */
Hypercube::~Hypercube() {
    delete[] this->vertices;
    delete[] this->f;
    delete this->h;
}