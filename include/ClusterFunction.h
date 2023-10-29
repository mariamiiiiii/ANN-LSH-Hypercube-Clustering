#pragma once

#include <vector>
#include <unordered_map>

#include "Image.h"

using namespace std;

/* Metric (2 for Euclidean) */
#define Metric 2

/* Class that keeps all the important information of Cluster, with all the required functions */
class Cluster {
    private:
        /*Cluster's id*/
        unsigned int id;

        /*Cluster's centroid*/
        Image *centroid;

        /*The points a cluster consists of*/
        unordered_map<int,Image*> points;
    public:
        /*Cluster's Constructor*/
        Cluster(Image &centroid,unsigned int id);

        /*Returns cluster's id*/
        unsigned int getId();

        /*Adds a point in the cluster's points*/
        bool addPoint(Image* point);

        /*Removes a point from the cluster's points*/
        bool removePoint(int id);

        /*Returns the size of the map which contains the cluster's points*/
        unsigned int getSize();

        /*Updates the centroid when needed*/
        void updateCentroid();

        /*Erases all the elements from the unordered map*/
        void clear();

        /*Returns the cluster's centroid*/
        Image* getCentroid();

        /*Returns all the cluster's points*/
        vector<Image*> getPoints();

        /*Measures the average distance between a given point and all the cluster's points*/
        double averageDistance(Image *point);

        /*Cluster's Destructor*/
        ~Cluster();
};