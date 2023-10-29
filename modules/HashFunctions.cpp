#include <iostream>
#include <random>

#include "../include/HashFunctions.h"
#include "../include/Image.h"


/* Constructor of Hash Function */
Hash_Function::Hash_Function(unsigned long k, unsigned long w, unsigned long dimension) {
    // Initialize parameters
    this->k = k;
    this->w = w;
    this->dimension = dimension;
    
    // Initialize uniform random distribution number generator
    default_random_engine generator;
    uniform_real_distribution<double> uniform_distribution(0, this->w - 0.001);

    // For each hash function generate it's Si's, 0 <= i <= d - 1
    this->v = new double*[k];
    for (unsigned long i = 0; i < this->k; i++) {
        this->v[i] = new double[dimension];
        for (unsigned long j = 0; j < this->dimension; j++) {
            this->v[i][j] = uniform_distribution(generator);
        }
    }
}


/* Creates h Hash Function */
unsigned long Hash_Function::h_function(Image *image, unsigned long k) {
    /* Random Number Generation */
    random_device rd;
    mt19937 gen(rd());

    /* Range of the uniform real distribution */
    double lower_bound = 0.0;   // lower value
    double upper_bound = w;     // higher value

    /* Create a uniform real distribution */
    uniform_real_distribution<double> distribution(lower_bound, upper_bound);

    /* Generate a random real number using the distribution */
    double w = this->w;
    double t = distribution(gen);

    /* projection value */
    double projection = 0.0;
    for (unsigned long j = 0; j < this->dimension; j++) {
        projection += image->getPixel(j) * this->v[k][j];
    }

    unsigned long cur_hash = static_cast<unsigned long>((projection + t) / this->w);

    return cur_hash;
}


/* Function to Calculate Power */
unsigned long Hash_Function::power(int base, int exponent) {
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


/* Destructor of Hash Function */
Hash_Function::~Hash_Function() {
    for (unsigned long i = 0; i < this->k; i++)
        delete[] this->v[i];

    delete[] this->v;
}