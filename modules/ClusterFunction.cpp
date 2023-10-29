#include <algorithm>
#include <cmath>
#include <vector>

#include "../include/ClusterFunction.h"

using namespace std;

Cluster::Cluster(Image &centroid,unsigned int id) {
    this->centroid = new Image(centroid);
    this->id = id;
}

unsigned int Cluster::getId() {
    return this->id;
}


bool Cluster::addPoint(Image *point) {
    /*Put the point in the cluster if it is not already in it else don't put it*/
    if (this->points.find(point->getImageId()) == this->points.end()) {
        this->points[point->getImageId()] = point;
        return true;
    }

    return false;
}

bool Cluster::removePoint(int id) {
    /*Remove the point of the cluster if it is in it*/
    if (this->points.find(id) != this->points.end()) {
        this->points.erase(id);
        return true;
    }
    
    return false;
}

unsigned int Cluster::getSize() {
    return this->points.size();
}


struct compareTwoPoints {
    compareTwoPoints(int dimention) {
        this->dimention = dimention; 
    }

    bool operator()(Image* p1, Image* p2) {
        return p2->getPixel(dimention) > p1->getPixel(dimention);
    }

    int dimention;
};

void Cluster::updateCentroid() {
    for (int j = 0; j < this->centroid->getImageSize(); j++) {
        vector<Image*> sortedPoints;
        for (auto it : this->points) { 
            sortedPoints.push_back(it.second);
        } 
        sort(sortedPoints.begin(), sortedPoints.end(), compareTwoPoints(j));
        /*Update the centroid to be the median of all the cluster's points*/
        this->centroid->setPixel(j, sortedPoints[(int) ceil(this->points.size() / 2)]->getPixel(j));
    }
}

void Cluster::clear() {
    this->points.clear();
}

Image* Cluster::getCentroid() {
    return this->centroid;
}

vector<Image*> Cluster::getPoints() {
    vector<Image*> resultVector;
    
    for (auto it : this->points) {
        resultVector.push_back(it.second);
    }

    return resultVector;
}

double Cluster::averageDistance(Image *point) {
    double distance = 0.0;

    for (auto it : this->points) {
        distance = distance + point->distance(it.second, 2);
    }

    double average = distance / this->points.size();
    return average;
}


Cluster::~Cluster() {
    delete this->centroid;
}