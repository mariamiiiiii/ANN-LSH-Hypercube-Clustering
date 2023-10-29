#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <random>
#include <unordered_set>
#include <algorithm>
#include <utility>
#include <ctime>

#include "include/InputHandling.h"
#include "include/Dataset.h"
#include "include/ClusterFunction.h"
#include "include/BruteforceSearch.h"
#include "include/lshFunctions.h"
#include "include/HypercubeFunction.h"

using namespace std;

/* Number of images to take from the Query File */
#define INPUT_IMAGES 50


double minDistanceBetweenCentroids(vector<Cluster*> &clusters) {
    // Calculate min distance between centers to use it as start radius for reverse search
    double result = 1.0/0.0;
    for(unsigned int i = 0; i < clusters.size(); i++) {
        for(unsigned int j = i + 1; j < clusters.size(); j++) {
            double distance = clusters[i]->getCentroid()->distance(clusters[j]->getCentroid(), 2);
            if(distance < result)
                result = distance;
        }
    }
    return result;
}


/* main function */
int main(int argc, char *argv[]) {

    /* Useful Variables */
    int value;
    string variable;
    bool continue_program = true;

    /* Default Variables for Configuration File */
    int K = 10;
    int L = 3;
    int kLSH = 4;
    int M_cube = 10;
    int kHypercube = 3;
    int probes = 2; 

    /* Variables for arguments */
    string input_file, configuration_file, output_file;
    int complete;
    string method;


    /* Check the arguments that the user gave */
    if (check_args(argc, argv, "hupercube") == -1) {
        return 0;
    }
    /* Parse the arguments from the user */
    else if (cluster_get_input(argc, argv, &input_file, &configuration_file, &output_file, &complete, &method)) {
        return 0;
    }


    cout << "-------------------------------------------" << endl;
    cout << "Input File: " << input_file << endl;
    cout << "Configuration File: " << configuration_file << endl;
    cout << "Output File: " << output_file << endl;
    cout << "complete: " << complete << endl;
    cout << "method: " << method << endl << endl;


    /* Open Configuration File */
    ifstream config_ifs(configuration_file);


    /* Read from Configuration File */
    while(config_ifs >> variable >> value) {
        if(variable == "number_of_clusters:") {
            K = value;
        }
        else if(variable == "number_of_vector_hash_tables:") {
            L = value;
        }
        else if(variable == "number_of_vector_hash_functions:") {
            kLSH = value;
        }
        else if(variable == "max_number_M_hypercube:") {
            M_cube = value;
        }
        else if(variable == "number_of_hypercube_dimensions:") {
            kHypercube = value;
        }
        else if(variable == "number_of_probes:") {
            probes = value;
        }
        else {
            cout << "Invalid Configuration File." << endl;
            return 0;
        }
    }


    /* Close Configuration File */
    config_ifs.close();

    /* Open Output File */ 
    ofstream outputStream(output_file);

    bool repeat;
    do {
        
        Dataset *dataset = new Dataset(input_file);

        if (dataset->isValid()) {
            /*Get images from dataset*/
            // vector<Image*> images = dataset->getImages();
            vector<Image*> images = dataset->getNumImages(1000);

            /*Initialize uniform random distribution number generator*/
            default_random_engine generator;
            uniform_int_distribution<int> uniform_distribution(1,images.size());

            // k-Means++ initialization:
            // Choose a centroid uniformly at random (indexing ranges from 1 to n)
            unordered_set<int> centroids;
            centroids.insert(uniform_distribution(generator));
            for (int t = 1; t < K; t++) {
                // For all non-centroids i, let D(i) = min distance to some centroid, among t chosen centroids and calculate P(r) = sum{D(i), 0 <= i <= r}
                double *P = new double[images.size() - t + 1];
                int *non_cendroid_index = new int[images.size() - t + 1];
                P[0] = 0;
                // Calculate max{D(i)} for all non-centroids i
                unsigned long long maxDistance = 0;
                for (unsigned int i = 1,j = 0; j < images.size(); j++) {
                    // Check if jth point is not a centroid and if so, keep it's index , calculate D(i) and use it to calculate P(i) using prefix sum technique. Otherwise, continue to next point.
                    if (centroids.find(j+1) == centroids.end()) {
                        // j is not a centroid
                        // Compute D(i)
                        double Di = 1.0/0.0;
                        for (auto c : centroids) {
                            double distance = images[c-1]->distance(images[i-1],2);
                            if (distance < Di) {
                                Di = distance;
                            }
                        }
                        i++;
                        if (Di >= maxDistance) {
                            maxDistance = Di;
                        }
                    }
                }
                // i is 1-starting index for all non-centriods and j is 0-starting index for all points
                for (unsigned int i = 1, j = 0; j < images.size(); j++) {
                    // Check if jth point is not a centroid and if so, keep it's index , calculate D(i) and use it to calculate P(i) using prefix sum technique. Otherwise, continue to next point.
                    if (centroids.find(j+1) == centroids.end()) {
                        // j is not a centroid
                        // Compute D(i)
                        double Di = 1.0/0.0;
                        for (auto c : centroids) {
                            double distance = images[c-1]->distance(images[i-1],2);
                            if (distance < Di) {
                                Di = distance;
                            }
                        }
                        Di = Di / maxDistance;
                        P[i] = P[i-1] + (Di * Di);
                        non_cendroid_index[i] = j+1;
                        i++;
                    }
                }
                
                // Choose new centroid: r chosen with probability proportional to D(r)^2
                uniform_real_distribution<float> floatDistribution(0,P[images.size() - t]);

                // Pick a uniformly distributed float x ∈ [0,P(n−t)] and return r ∈ {1,2,...,n−t} : P(r−1) < x ≤ P(r), where P(0) = 0.
                float x = floatDistribution(generator);
                int left = 1, right = images.size() - t,r = 0;
                // Find r using binary search to P
                while (left <= right) {
                    r = (left+right)/2;
                    if (P[r-1] < x && x <= P[r]) { // P[r-1] < x <= P[r]
                        break;
                    }
                    else if (x <= P[r-1]) {  // x <= P[r-1] <=  P[r]
                        right = r - 1;
                    }
                    else {  // P[r-1] <= P[r] < x
                        left = r + 1;
                    }
                }
                // Add chosen centroid r to centroids set
                centroids.insert(non_cendroid_index[r]);

                delete[] non_cendroid_index;
                delete[] P;
            }
            // Initialize clusters for all centroids
            vector<Cluster*> clusters;
            unsigned int cId = 0;
            for (auto c : centroids) {
                clusters.push_back(new Cluster(*images[c-1],cId++));
            }

            unsigned int assignments;
            unordered_map<int, Cluster*> clusterHistory;

            int w = 0;
            LSH *lsh = NULL;
            Hypercube *hypercube = NULL;
            unordered_map<int,Image*> pointsMap;
            if (method == "LSH" || method == "Hypercube") {
                w = dataset->avg_NN_distance() * 6;
                // Initialize LSH or Hypercube interface
                if (method == "LSH") {
                    lsh = new LSH(kLSH,w,L,dataset);
                }
                else {
                    hypercube = new Hypercube(dataset, kHypercube, w);
                }
                // Create an unordered map with all the points to help in the reverse assignment step
                for (unsigned int i = 0; i < images.size(); i++) {
                    pointsMap[images[i]->getImageId()] = images[i];
                }
            }





            // Clustering time!!!
            clock_t begin_clustering_time = clock();
            do {
                for (unsigned int i = 0; i < clusters.size(); i++) {
                    clusters[i]->clear();
                }
                assignments = 0;
                // Assignment step
                if (method == "Classic") {
                        
                    // Lloyd's algorithn
                    for (unsigned int i = 0; i < images.size(); i++) {
                        // Find closest cluster for the current(ith) image
                        double minDistance = 1.0/0.0;
                        Cluster *minCluster = NULL;
                        for (unsigned j = 0; j < clusters.size(); j++) {
                            double distance = images[i]->distance(clusters[j]->getCentroid(),2);
                            if (distance < minDistance) {
                                minDistance = distance;
                                minCluster = clusters[j];
                            }
                        }
                        // Insert the ith image to it's closest cluster
                        minCluster->addPoint(images[i]);
                        if (clusterHistory.find(images[i]->getImageId()) == clusterHistory.end() || clusterHistory[images[i]->getImageId()]->getId() != minCluster->getId()) {
                            assignments++;
                        }
                        clusterHistory[images[i]->getImageId()] = minCluster;
                    }

                } else if (method == "LSH") {
                    // LSH Reverse Assignment
                    unordered_map<int,Image*> tmpPointsMap = pointsMap;
                    unordered_map<int,Cluster*> bestCluster;
                    double R = ceil(minDistanceBetweenCentroids(clusters)/2.0);
                    unsigned int curPoints = 0,prevPoints;
                    do {
                        prevPoints = curPoints;
                        curPoints = 0;
                        // Range search on all cluster centroids and find the best cluster for all range-searched points
                        for (unsigned int i = 0; i < clusters.size(); i++) {
                            vector<Image*> pointsInRange = lsh->rangeSearch(clusters[i]->getCentroid(),R);
                            curPoints += pointsInRange.size();
                            for (unsigned int j = 0; j < pointsInRange.size(); j++) {
                                // Check if current in-range point was not yet assigned to a cluster
                                if (tmpPointsMap.find(pointsInRange[j]->getImageId()) != tmpPointsMap.end()) {
                                    // If so, assign it to the corresponding cluster
                                    bestCluster[pointsInRange[j]->getImageId()] = clusters[i];
                                    tmpPointsMap.erase(pointsInRange[j]->getImageId());
                                } else if (pointsInRange[j]->distance(clusters[i]->getCentroid(),2) < pointsInRange[j]->distance(bestCluster[pointsInRange[j]->getImageId()]->getCentroid(),2)) {
                                    bestCluster[pointsInRange[j]->getImageId()] = clusters[i];
                                }
                            }
                        }
                        R *= 2.0;
                    } while (curPoints - prevPoints > 0);
                    // Assign all range-searched points to their best cluster
                    for (auto it : bestCluster) {
                        it.second->addPoint(pointsMap[it.first]);
                        if (clusterHistory.find(it.first) == clusterHistory.end() || clusterHistory[it.first]->getId() != it.second->getId()) {
                            assignments++;
                        }
                        clusterHistory[it.first] = it.second;
                    }
                    // Assign rest points using Lloyd's method
                    for(auto it : tmpPointsMap) {
                        double distanceToClosestCentroid = 1.0/0.0;
                        Cluster *closestCluster = NULL;

                        for (unsigned int i = 0; i < clusters.size(); i++) {
                            double distance = it.second->distance(clusters[i]->getCentroid(),2);

                            if (distance < distanceToClosestCentroid) {
                                distanceToClosestCentroid = distance;
                                closestCluster = clusters[i];
                            }
                        }
                        // Insert the current image to it's closest cluster
                        closestCluster->addPoint(it.second);
                        if (clusterHistory.find(it.first) == clusterHistory.end() || clusterHistory[it.first]->getId() != closestCluster->getId()) {
                            assignments++;
                        }

                        clusterHistory[it.first] = closestCluster;
                    }
                } else if (method == "Hypercube") {
                    // Hypercube Reverse Assignment
                    unordered_map<int,Image*> tmpPointsMap = pointsMap;
                    unordered_map<int,Cluster*> bestCluster;
                    double R = ceil(minDistanceBetweenCentroids(clusters)/2.0);
                    unsigned int curPoints = 0,prevPoints;
                    do {
                        prevPoints = curPoints;
                        curPoints = 0;
                        // Range search on all cluster centroids and assign the returned in-range points
                        cout << clusters.size() << endl;
                        for (unsigned int i = 0; i < clusters.size(); i++) {
                            list<Image*> pointsInRange = hypercube->rangeSearch(clusters[i]->getCentroid(), M_cube, probes,R);
                            curPoints = curPoints + pointsInRange.size();
                            cout << i << endl;
                            for (list<Image*>::iterator it = pointsInRange.begin(); it != pointsInRange.end(); it++) {
                                // Check if current in-range point was not yet assigned to a cluster
                                if (tmpPointsMap.find((*it)->getImageId()) != tmpPointsMap.end()) {
                                    // If so, assign it to the corresponding cluster
                                    bestCluster[(*it)->getImageId()] = clusters[i];
                                    tmpPointsMap.erase((*it)->getImageId());
                                } else if ((*it)->distance(clusters[i]->getCentroid(),2) < (*it)->distance(bestCluster[(*it)->getImageId()]->getCentroid(),2)) {
                                    bestCluster[(*it)->getImageId()] = clusters[i];
                                }
                            }
                        }

                        R *= 2.0;
                    } while (curPoints - prevPoints > 0);
                    // Assign all range-searched points to their best cluster
                    for (auto it : bestCluster) {
                        it.second->addPoint(pointsMap[it.first]);
                        if (clusterHistory.find(it.first) == clusterHistory.end() || clusterHistory[it.first]->getId() != it.second->getId()) {
                            assignments++;
                        }
                        clusterHistory[it.first] = it.second;
                    }
                    // Assign rest points using Lloyd's method
                    for(auto it : tmpPointsMap) {
                        double distanceToClosestCentroid = 1.0/0.0;
                        Cluster *closestCluster = NULL;
                        
                        for (unsigned int i = 0; i < clusters.size(); i++) {
                            double distance = it.second->distance(clusters[i]->getCentroid(),2);
                            if (distance < distanceToClosestCentroid) {
                                distanceToClosestCentroid = distance;
                                closestCluster = clusters[i];
                            }
                        }
                        // Insert the current image to it's closest cluster
                        closestCluster->addPoint(it.second);
                        if (clusterHistory.find(it.first) == clusterHistory.end() || clusterHistory[it.first]->getId() != closestCluster->getId()) {
                            assignments++;
                        }
                        clusterHistory[it.first] = closestCluster;
                    }





                }

                // Update all cluster centroids
                if (assignments > 0) {
                    for (unsigned int i = 0; i < clusters.size(); i++) {
                        clusters[i]->updateCentroid();
                    }
                }

                
            } while (assignments > 100);
            double clustering_time = double(clock() - begin_clustering_time) / CLOCKS_PER_SEC;
            
            // Print used method
            outputStream << "Algorithm: ";
            if (method == "Classic") {
                outputStream << "Lloyds";
            } else if (method == "LSH") {
                outputStream << "Range Search LSH";
            } else if (method == "Hypercube") {
                outputStream << "Range Search Hypercube";
            }
            outputStream << endl;

            // Print stats
            for (unsigned int i = 0; i < clusters.size(); i++) {
                outputStream << "CLUSTER-" << i+1 << " {size: " << clusters[i]->getSize() << ", centroid: [";
                for (int j = 0; j < dataset->getImageSize() - 1; j++) {
                    outputStream << (int)clusters[i]->getCentroid()->getPixel(j) << ", ";
                }
                outputStream << (int)clusters[i]->getCentroid()->getPixel(dataset->getImageSize()-1) << "]}\n";
            }

            // Print clustering time
            outputStream << "clustering_time: " << clustering_time << endl;

            // Calculate Silhouette for all images
            double averageSilhouette = 0.0;
            vector<double> s;
            for (unsigned int i = 0; i < images.size(); i++) {
                // Calculate distance of ith image to all the clusters
                Cluster *neighbourCluster = NULL, *closestCluster = NULL;
                double distanceToClosestCentroid = 1.0/0.0, distanceToSecondClosest = 1.0/0.0;
                
                for (unsigned int j = 0; j < clusters.size(); j++) {
                    double distance = images[i]->distance(clusters[j]->getCentroid(), 2);

                    if (distance < distanceToClosestCentroid) {
                        distanceToSecondClosest = distanceToClosestCentroid;
                        distanceToClosestCentroid = distance;

                        neighbourCluster = closestCluster;
                        closestCluster = clusters[j];
                    }
                    else if(distance < distanceToSecondClosest) {
                        distanceToSecondClosest = distance;
                        neighbourCluster = clusters[j];
                    }
                }
                // Calculate average distance of ith image to images in same cluster
                double ai = clusterHistory[images[i]->getImageId()]->averageDistance(images[i]);
                // Calculate average distance of ith image to images in the next best(neighbor) cluster
                double bi = neighbourCluster->averageDistance(images[i]);
                // Calculate Silhouette for ith image
                double si = (bi - ai)/max(ai, bi);
                s.push_back(si);
                averageSilhouette += si;
            }
            // Print Silhouettes
            outputStream << "Silhouette: [";

            // Calculate and print average Silhouette for each cluster
            for (unsigned int i = 0; i < clusters.size(); i++) {
                double averageS = 0.0;
                vector<Image*> clusterPoints = clusters[i]->getPoints();
                for (unsigned int j = 0; j < clusterPoints.size(); j++) {
                    averageS += averageS + s[clusterPoints[j]->getImageId()];
                }
                outputStream << averageS/clusterPoints.size() << ",";
            }
            
            // Print average Silhouette for all points in dataset
            averageSilhouette = averageSilhouette / images.size();
            outputStream << " " << averageSilhouette << "]\n";

            // Optionally (with command line parameter –complete) print image numbers in each cluster
            if (complete) {
                for (unsigned int i = 0; i < clusters.size(); i++) {
                    outputStream << "CLUSTER-" << i+1 << " {[";
                    for (int j = 0; j < dataset->getImageSize() - 1; j++) {
                        outputStream << (int)clusters[i]->getCentroid()->getPixel(j) << ", ";
                    }
                    outputStream << (int)clusters[i]->getCentroid()->getPixel(dataset->getImageSize()-1) << "]";
                    vector<Image*> clusterImages = clusters[i]->getPoints();
                    for (unsigned int j = 0; j < clusterImages.size(); j++) {
                        outputStream << ", " << clusterImages[j]->getImageId();
                    }
                    outputStream << "}\n";
                }
            }

            // for (unsigned int i = 0; i < clusters.size(); i++) {
            //     delete clusters[i];
            // }
            
            if (method == "LSH") {
                delete lsh;
            }
            if (method == "Hypercube") {
                delete hypercube;
            }
        }
        
        delete dataset;

        //outputStream << "Do you want to enter another query (Y/N)?  ";
        string Answer;
        cout << "Do you want to enter another query (Y/N)?  ";

        do {
            cin >> Answer;
        } while(Answer != "Y" && Answer != "N" && Answer != "y" && Answer != "n" && cout << "Invalid answer" << endl);

        repeat = (Answer == "Y") | (Answer == "y");

        if(repeat) {
            cout << "Input File: ";
            cin >> input_file;
        }

    } while(repeat);

    outputStream.close();

    return 0;
}

