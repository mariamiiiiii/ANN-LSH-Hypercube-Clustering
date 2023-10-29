#pragma once

#include <random>

#include "Image.h"

using namespace std;

/* Class that keeps all the important information of Hash Function, with all the required functions */
class Hash_Function {
    private:
        unsigned long w;            // window
        unsigned long k;            // num of LSH functions
        unsigned long dimension;    // dimension of the images
        double **v;
    public:
        /* Constructor of Hash Function */
        Hash_Function(unsigned long k, unsigned long w, unsigned long d);
        
        /* Creates h Hash Function */
        unsigned long h_function(Image *image, unsigned long k);

        /* Function to Calculate Power */
        unsigned long power(int base, int exponent);

        /* Destructor of Hash Function */
        ~Hash_Function();
};
