#pragma once

typedef unsigned char Pixel;


/* Class that keeps all the important information an Image, with all the required functions */
class Image {
    private:
        int id;
        int width;
        int height;
        Pixel *pixels;
    public:
        /* Constructor of Image */
        Image(int id, int width, int height);

        /* Set pixen with index */
        bool setPixel(int index, Pixel pixel);

        /* Returns the id of the image */
        int getImageId();

        /* Returns the pixel value at a specific index in the image */
        Pixel getPixel(int index);

        /* Returns the size of the image */
        int getImageSize();

        // Calculates the p-norm distance to another image
        double distance(Image *image, int metric);

        /* Function to Calculate Power */
        unsigned long power(int base, int exponent);

        /* Returns the width of image */
        int getWidth() const {
            return width;
        }

        /* Returns the height of image */
        int getHeight() const {
            return height;
        }

        /* Destructor of Dataset */
        ~Image();
};
