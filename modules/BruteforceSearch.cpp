#include <algorithm>

#include "../include/BruteforceSearch.h"

/* Constructor of Bruteforce */
Bruteforce::Bruteforce(vector<Image*> images) {
    this->images = images;
}

bool comparePairs(const pair<double, int>& a, const pair<double, int>& b) {
    return a.first < b.first;
}

/* Perform exact nearest neighbor search using a brute-force approach */
vector<pair<double, int>> Bruteforce::exactNN(Image* query, int N) {
    /* Rtore the distances and image IDs of nearest neighbors */
    vector<pair<double, int>> neighbors;

    /* Loop through all the images */
    for (unsigned i = 0; i < this->images.size(); i++) {
        double sumOfSquares = 0.0;
        for (int j = 0; j < (28*28); j++) {
            double diff = query->getPixel(j) - this->images[i]->getPixel(j);
            sumOfSquares += diff * diff;
        }
        double distance = sqrt(sumOfSquares);

        pair<double, int> temp(distance, this->images[i]->getImageId());
        neighbors.push_back(temp);
    }

    sort(neighbors.begin(), neighbors.end(), comparePairs);

    if (neighbors.size() > N) {
        neighbors.resize(N);
    }

    return neighbors;
}




