#include <algorithm>
#include <cmath>
#include <vector>

#include "../include/ClusterFunction.h"

using namespace std;

/*Cluster's Constructor*/
Cluster::Cluster(Image &centroid,unsigned int id) {
    this->centroid = new Image(centroid);
    this->id = id;
}


/*Returns cluster's id*/
unsigned int Cluster::getId() {
    return this->id;
}


/*Adds a point in the cluster's points*/
bool Cluster::addPoint(Image *point) {
    /*Put the point in the cluster if it is not already in it else don't put it*/
    if (this->points.find(point->getImageId()) == this->points.end()) {
        this->points[point->getImageId()] = point;
        return true;
    }

    return false;
}


/*Removes a point from the cluster's points*/
bool Cluster::removePoint(int id) {
    /*Remove the point of the cluster if it is in it*/
    if (this->points.find(id) != this->points.end()) {
        this->points.erase(id);
        return true;
    }
    
    return false;
}


/*Returns the size of the map which contains the cluster's points*/
unsigned int Cluster::getSize() {
    return this->points.size();
}


struct compareTwoPoints {
    compareTwoPoints(int dimention) {
        this->dimention = dimention; 
    }

    /*Returns true if p2 is bigger*/
    bool operator()(Image* p1, Image* p2) {
        return p2->getPixel(dimention) > p1->getPixel(dimention);
    }

    int dimention;
};


/*Updates the centroid when needed*/
void Cluster::updateCentroid() {
    for (int j = 0; j < this->centroid->getImageSize(); j++) {
        vector<Image*> sortedPoints;
        /*for all the points of every centroid*/
        for (auto it : this->points) { 
            sortedPoints.push_back(it.second);
        } 
        /*Sort the points based on compareTwoPoints()*/
        sort(sortedPoints.begin(), sortedPoints.end(), compareTwoPoints(j));
        /*The centroid is the median of all the cluster's points*/
        this->centroid->setPixel(j, sortedPoints[(int) ceil(this->points.size() / 2)]->getPixel(j));
    }
}


/*Erases all the elements from the unordered map*/
void Cluster::clear() {
    this->points.clear();
}


/*Returns the cluster's centroid*/
Image* Cluster::getCentroid() {
    return this->centroid;
}

/*Returns all the cluster's points*/
vector<Image*> Cluster::getPoints() {
    vector<Image*> resultVector;
    
    for (auto it : this->points) {
        resultVector.push_back(it.second);
    }

    return resultVector;
}


/*Measures the average distance between a given point and all the cluster's points*/
double Cluster::averageDistance(Image *point) {
    double distance = 0.0;

    for (auto it : this->points) {
        distance = distance + point->distance(it.second, Metric);
    }

    double average = distance / this->points.size();
    return average;
}


/*Cluster's Destructor*/
Cluster::~Cluster() {
    delete this->centroid;
}