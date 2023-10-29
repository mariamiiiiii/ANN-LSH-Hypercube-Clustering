#pragma once

#include <vector>
#include <unordered_map>

#include "Image.h"

using namespace std;

class Cluster
{
    private:
        unsigned int id;
        Image *centroid;
        unordered_map<int,Image*> points;
    public:
        Cluster(Image &centroid,unsigned int id);
        unsigned int getId();
        bool addPoint(Image* point);
        bool removePoint(int id);
        unsigned int getSize();
        void updateCentroid();
        void clear();
        Image* getCentroid();
        vector<Image*> getPoints();
        double averageDistance(Image *point);
        ~Cluster();
};