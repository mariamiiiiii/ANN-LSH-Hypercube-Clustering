#include <cmath>
#include <iostream>

#include "../include/Image.h"

using namespace std;


/* Constructor of Image */
Image::Image(int id, int width, int height) {
    this->id = id;
    this->width = width;
    this->height = height;
    this->pixels = new Pixel[width * height];
}


/* Set pixen with index */
bool Image::setPixel(int index, Pixel pixel) {    
    /* Set pixel */ 
    this->pixels[index] = pixel;
    return true;
}


/* Returns the id of the image */
int Image::getImageId() {
    return this->id;
}


/* Returns the pixel value at a specific index in the image */
Pixel Image::getPixel(int index) {
    /* Check if the index is within valid range */ 
    if (index < (this->width * this->height) && index >= 0) {
        /* Return the pixel value at the specified index */
        return this->pixels[index];
    } 
    else {
        return -1;
    }
}


/* Returns the size of the image */
int Image::getImageSize() {
    return this->width * this->height;
}


/* Calculate the distance with the chosen metric */
double Image::distance(Image *image, int metric) {    
    double dist = 0.0;

    for (int i = 0; i < this->getImageSize(); i++) {
        /* Calculate the absolute difference */
        int absolute_diff = abs(this->getPixel(i) - image->getPixel(i));
        dist += power(absolute_diff, metric);
    }

    /* Calculate the square */
    unsigned long result = pow(dist, 1.0 / metric);

    return result;
}


/* Function to Calculate Power */
unsigned long Image::power(int base, int exponent) {
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


/* Destructor of Dataset */
Image::~Image() {
    delete[] this->pixels;
}