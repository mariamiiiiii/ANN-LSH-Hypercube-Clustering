#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <iomanip> 

#include "../include/Dataset.h"
#include "../include/BruteforceSearch.h"


/* Constructor of Dataset */
Dataset::Dataset(string input_file) {   

    /* Opent Input File */ 
    ifstream input_f(input_file, ios::out | ios::binary);

    /* Check if Input File is valid */
    if (!input_f) {
        cout << "The Input File is not valid!" << endl;
        this->valid = false;
    }
    else {
        /* Read header of MNIST Dataset */
        input_f.read((char*)&this->header,sizeof(Header));
        
        /* For big endianess */ 
        this->header.magic_num = SWAP_INT32(this->header.magic_num);
        this->header.num_images = SWAP_INT32(this->header.num_images);
        this->header.num_rows = SWAP_INT32(this->header.num_rows);
        this->header.num_columns = SWAP_INT32(this->header.num_columns);

        /* Read all Images of the input file */
        for (int i = 0; i < this->header.num_images; i++) {
            Pixel image[getImageSize()];

            input_f.read((char*)&image, sizeof(image));

            this->images.push_back(new Image(i, this->header.num_columns, this->header.num_rows));

            /* Read the pixels of every image */
            for (int p = 0; p < this->getImageSize(); p++) {
                this->images.at(i)->setPixel(p, image[p]);
            }
        }

        /* Close Input File */
        input_f.close();

        this->valid = true;

        cout << this->header.num_images << " images were read for: " << input_file << endl;
    }
}


/* Returns if Dataset is valid (opened correctly) */
bool Dataset::isValid() {
    return this->valid;
}


/* Returns the total pixels of image in dataset (always 28*28 = 784 for MNIST Dataset) */
int Dataset::getImageSize() {
    return this->header.num_rows * this->header.num_columns;
}


/* Returns a vector with all the Images of the Dataset */
vector<Image*> Dataset::getImages(){
    return this->images;
}

/* Retunt the first 'num' images of the Dataset */
vector<Image*> Dataset::getNumImages(int num) {
    vector<Image*> result;

    if (num <= 0 || num > images.size()) {
        cout << "Invalid number of images requested." << endl;
        return result; 
    }

    for (int i = 0; i < num; ++i) {
        result.push_back(images[i]);
    }

    return result;
}


/* Print all images of Dataset (Image ID, Width, Height, Pixels) */
void Dataset::printImages() {
    for (Image *image : images) {
        cout << "Image ID: " << image->getImageId() << endl;
        cout << "Width: " << image->getWidth() << ", Height: " << image->getHeight() << endl;

        /* Print pixels with 28 pixels per row */ 
        for (int i = 0; i < image->getImageSize(); ++i)
        {
            cout << setw(4) << static_cast<int>(image->getPixel(i)) << " ";
            if ((i + 1) % 28 == 0)
            {
                cout << endl;
            }
        }

        cout << endl;
    }
}


/* Print Selected images from Dataset */
void Dataset::printSelectedImages(const vector<Image*>& selectedImages) {
    for (Image* image : selectedImages) {
        cout << "Image ID: " << image->getImageId() << endl;
        cout << "Width: " << image->getWidth() << ", Height: " << image->getHeight() << endl;

        // Print pixels with 28 pixels per row
        for (int i = 0; i < image->getImageSize(); ++i) {
            cout << setw(4) << static_cast<int>(image->getPixel(i)) << " ";
            if ((i + 1) % 28 == 0) {
                cout << endl;
            }
        }

        cout << endl;
    }
}


int Dataset::avg_NN_distance() {
    int step = images.size() / W_SAMPLE_SIZE;
    double dist_sum = 0.0;
    Bruteforce bf(images);

    for(unsigned int i = 0; i < images.size(); i += step) {
        dist_sum += bf.exactNN(images[i], 2)[1].first;
    }

    return dist_sum / W_SAMPLE_SIZE;
}


/* Destructor of Dataset */
Dataset::~Dataset() {
    for (std::vector<Image*>::iterator it = this->images.begin();it < this->images.end();it++) {
        delete *it;
    }
}