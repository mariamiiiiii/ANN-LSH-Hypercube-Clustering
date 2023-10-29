#pragma once

#include <string>
#include <vector>

#include "image.h"

using namespace std;

#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define W_SAMPLE_SIZE 50

/* Class that keeps all the important information of a Dataset with all the required functions */
class Dataset {
    private:
        /* Header of image dataset */
        struct Header {
            unsigned int magic_num;
            unsigned int num_images;
            unsigned int num_rows;
            unsigned int num_columns;
        };
        Header header;

        /* Validation of Dataset */
        bool valid;

        /* Imaged of MNIST Dataset */
        vector<Image*> images;

    public:
        /* Constructor of Dataset */
        Dataset(string input_file);

        /* Returns if Dataset is valid (opened correctly) */
        bool isValid();

        /* Returns the total pixels of image in dataset (always 28*28 = 784 for MNIST Dataset) */
        int getImageSize();

        /* Returns a vector with all the Images of the Dataset */
        vector<Image*> getImages();
        
        /* Retunt the first 'num' images of the Dataset */
        vector<Image*> getNumImages(int num);

        /* Print all images of Dataset (Image ID, Width, Height, Pixels) */
        void printImages();

        /* Print Selected images from Dataset */
        void printSelectedImages(const vector<Image*>& selectedImages);

        // Used to approximate good value of w
        int avg_NN_distance();

        /* Destructor of Dataset */
        ~Dataset();

};

