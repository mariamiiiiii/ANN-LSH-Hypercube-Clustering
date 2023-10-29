#pragma once

#include <vector>
#include <iostream>
#include <cmath>

#include "Image.h"

using namespace std;

/* Class that keeps all the important information of Bruteforce, with all the required functions */
class Bruteforce {
    private:
        vector<Image*> images;

    public:
        /* Constructor of Bruteforce */
        Bruteforce(vector<Image*> images);
        
        /* Perform exact nearest neighbor search using a brute-force approach */
        vector<pair<double, int>> exactNN(Image *query, int N);
};
