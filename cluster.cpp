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

/* Window (usually [2, 6]) */
#define W 6

/* Metric (2 for Euclidean) */
#define Metric 2


/* Returns the minimum distance between centroids*/
double min_dist(vector<Cluster*> &clusters) {
    // Calculate min distance between centers to use it as start radius for reverse search
    double result = 1.0/0.0;
    for(unsigned int i = 0; i < clusters.size(); i++) {
        for(unsigned int j = i + 1; j < clusters.size(); j++) {
            double distance = clusters[i]->getCentroid()->distance(clusters[j]->getCentroid(), Metric);
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


    /* Run this loop until the user stop it */
    while (continue_program == true) {
        
        /* Initialize the dataset from the Input File */
        Dataset *input_dataset = new Dataset(input_file);

        /* Check if input_file is valid */
        if (!input_dataset->isValid()) {
            cout << "Invalid dataset: " << input_file << endl;
            delete input_dataset;
            return 0;
        }

        /* Print out the images in the input_dataset if you want */ 
        // input_dataset->printImages();


        /*Get images from dataset*/
        // vector<Image*> images = dataset->getImages();
        vector<Image*> input_images = input_dataset->getNumImages(INPUT_IMAGES);

        /* Check if the query_images were taken correctly */
        if (input_images.empty()) {
            return 0;
        }

        /* Print out the query_images if you want */ 
        // queryset->printselectedImages(query_images);

        /*Initialize uniform random distribution number generator*/
        default_random_engine generator;
        uniform_int_distribution<int> uniform_distribution(1, input_images.size());

        /*k-Means++ initialization*/
        unordered_set<int> centroids;
        /*Random initialization for the first centroid*/
        centroids.insert(uniform_distribution(generator));

        for (int t = 1; t < K; t++) {
            /*For all non-centroids i, let Di = min distance to some centroid*/
            double *P = new double[input_images.size() - t + 1];
            int *non_cendroid_index = new int[input_images.size() - t + 1];
            P[0] = 0;

            /*Calculate maxDi for all non-centroids i*/
            unsigned long long maxDistance = 0;

            /* For every image in input_images */
            for (unsigned int i = 1,j = 0; j < input_images.size(); j++) {
                /*If jth point is not a centroid, keep it's index, calculate Di and use it to calculate P. If it is, continue to next point*/
                if (centroids.find(j + 1) == centroids.end()) {
                    /*j is not a centroid, compute Di*/
                    double Di = 1.0 / 0.0;

                    for (auto c : centroids) {
                        double distance = input_images[c-1]->distance(input_images[i - 1], Metric);
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

            /*i is for all non-centriods and starts with 1 and j is for all points and starts with 0*/
            for (unsigned int i = 1, j = 0; j < input_images.size(); j++) {
                /*If jth point is not a centroid, keep it's index, calculate Di and use it to calculate P. If it is, continue to next point*/
                if (centroids.find(j+1) == centroids.end()) {
                    /*j is not a centroid, compute Di*/
                    double Di = 1.0 / 0.0;
                    for (auto c : centroids) {
                        double distance = input_images[ c- 1]->distance(input_images[i - 1], Metric);
                        if (distance < Di) {
                            Di = distance;
                        }
                    }

                    Di = Di / maxDistance;

                    P[i] = P[i - 1] + (Di * Di);

                    non_cendroid_index[i] = j + 1;

                    i++;
                }
            }
            
            /*Choose new centroid: r chosen with probability proportional to D(r)^2*/
            uniform_real_distribution<float> floatDistribution(0, P[input_images.size() - t]);

            /* Pick a uniformly distributed float x */
            float x = floatDistribution(generator);
            int left = 1, right = input_images.size() - t,r = 0;
            
            while (left <= right) {
                r = (left + right) / Metric;
                if (P[r - 1] < x && x <= P[r]) { 
                    break;
                }
                else if (x <= P[r-1]) { 
                    right = r - 1;
                }
                else { 
                    left = r + 1;
                }
            }
            
            
            /*Add chosen centroid r to centroids set*/
            centroids.insert(non_cendroid_index[r]);

            delete[] non_cendroid_index;
            delete[] P;
        }


        /* Initialize clusters for all centroids */ 
        vector<Cluster*> clusters;
        unsigned int cId = 0;
        for (auto c : centroids) {
            clusters.push_back(new Cluster(*input_images[c-1], cId++));
        }

        unsigned int assignments;
        unordered_map<int, Cluster*> clusterHistory;


        LSH *lsh = NULL;
        Hypercube *hypercube = NULL;
        
        unordered_map<int,Image*> pointsMap;

        if (method == "LSH" || method == "Hypercube") {
            /* Initialize LSH interface */
            if (method == "LSH") {
                lsh = new LSH(kLSH, W, L, input_dataset);
            }
            /* Initialize Hypercube interface */
            else {
                hypercube = new Hypercube(input_dataset, kHypercube, W);
            }
            /*Create an unordered map with all the points for the reverse assignment step*/
            for (unsigned int i = 0; i < input_images.size(); i++) {
                pointsMap[input_images[i]->getImageId()] = input_images[i];
            }
        }


        /* Execute Clustering */ 
        clock_t begin_clustering_time = clock();
        do {
            for (unsigned int i = 0; i < clusters.size(); i++) {
                clusters[i]->clear();
            }

            assignments = 0;
            
            /*Classic method*/
            if (method == "Classic") {
                    
                /*Lloyd's algorithn*/
                for (unsigned int i = 0; i < input_images.size(); i++) {
                    /*Find closest cluster for the current(ith) image*/
                    double minDistance = 1.0 / 0.0;
                    Cluster *minCluster = NULL;

                    for (unsigned j = 0; j < clusters.size(); j++) {
                        double distance = input_images[i]->distance(clusters[j]->getCentroid(), Metric);
                        if (distance < minDistance) {
                            minDistance = distance;
                            minCluster = clusters[j];
                        }
                    }
                    
                    /*Insert the ith image to it's closest cluster*/
                    minCluster->addPoint(input_images[i]);
                    if (clusterHistory.find(input_images[i]->getImageId()) == clusterHistory.end() || clusterHistory[input_images[i]->getImageId()]->getId() != minCluster->getId()) {
                        assignments++;
                    }

                    clusterHistory[input_images[i]->getImageId()] = minCluster;
                }

            } 
            /*LSH method*/
            else if (method == "LSH") {
                unordered_map<int,Image*> tmpPointsMap = pointsMap;
                unordered_map<int,Cluster*> bestCluster;
                double R = ceil(min_dist(clusters) / 2.0);
                unsigned int curPoints = 0,prevPoints;
                do {
                    prevPoints = curPoints;
                    curPoints = 0;
                    
                    /*Range search on all cluster centroids and find the best cluster for all in-range points*/
                    for (unsigned int i = 0; i < clusters.size(); i++) {
                        vector<Image*> pointsInRange = lsh->rangeSearch(clusters[i]->getCentroid(),R);
                        curPoints += pointsInRange.size();

                        for (unsigned int j = 0; j < pointsInRange.size(); j++) {

                            /*Check if current in-range point was assigned to a cluster yet or not*/
                            if (tmpPointsMap.find(pointsInRange[j]->getImageId()) != tmpPointsMap.end()) {
                                
                                /*If it isn't, assign it to the corresponding cluster*/
                                bestCluster[pointsInRange[j]->getImageId()] = clusters[i];
                                tmpPointsMap.erase(pointsInRange[j]->getImageId());
                            } 
                            else if (pointsInRange[j]->distance(clusters[i]->getCentroid(), Metric) < pointsInRange[j]->distance(bestCluster[pointsInRange[j]->getImageId()]->getCentroid(), Metric)) {
                                bestCluster[pointsInRange[j]->getImageId()] = clusters[i];
                            }
                        }
                    }

                    R = R * 2.0;

                } while (curPoints - prevPoints > 0);
                
                
                /*Assign all range-searched points to their best cluster*/
                for (auto it : bestCluster) {
                    it.second->addPoint(pointsMap[it.first]);

                    if (clusterHistory.find(it.first) == clusterHistory.end() || clusterHistory[it.first]->getId() != it.second->getId()) {
                        assignments++;
                    }

                    clusterHistory[it.first] = it.second;
                }
                
                
                for(auto it : tmpPointsMap) {
                    double distanceToClosestCentroid = 1.0 / 0.0;
                    Cluster *closestCluster = NULL;

                    for (unsigned int i = 0; i < clusters.size(); i++) {
                        double distance = it.second->distance(clusters[i]->getCentroid(), Metric);

                        if (distance < distanceToClosestCentroid) {
                            distanceToClosestCentroid = distance;
                            closestCluster = clusters[i];
                        }
                    }
                    
                    /*Insert the current image to it's closest cluster*/
                    closestCluster->addPoint(it.second);

                    if (clusterHistory.find(it.first) == clusterHistory.end() || clusterHistory[it.first]->getId() != closestCluster->getId()) {
                        assignments++;
                    }

                    clusterHistory[it.first] = closestCluster;
                }
            } 
            /*Hypercube method*/
            else if (method == "Hypercube") {
                // Hypercube Reverse Assignment
                unordered_map<int,Image*> tmpPointsMap = pointsMap;
                unordered_map<int,Cluster*> bestCluster;
                double R = ceil(min_dist(clusters) / 2.0);
                unsigned int curPoints = 0,prevPoints;

                do {
                    prevPoints = curPoints;
                    curPoints = 0;
                    /*Range search on all cluster centroids and assign the returned in-range points*/
                    cout << clusters.size() << endl;
                    for (unsigned int i = 0; i < clusters.size(); i++) {
                        list<Image*> pointsInRange = hypercube->rangeSearch(clusters[i]->getCentroid(), M_cube, probes,R);
                        curPoints = curPoints + pointsInRange.size();
                        

                        for (list<Image*>::iterator it = pointsInRange.begin(); it != pointsInRange.end(); it++) {
                            /*Check if current in-range point was assigned to a cluster yet or not*/
                            if (tmpPointsMap.find((*it)->getImageId()) != tmpPointsMap.end()) {
                                
                                /*Check if current in-range point was assigned to a cluster yet or not*/
                                bestCluster[(*it)->getImageId()] = clusters[i];
                                tmpPointsMap.erase((*it)->getImageId());
                            } else if ((*it)->distance(clusters[i]->getCentroid(), Metric) < (*it)->distance(bestCluster[(*it)->getImageId()]->getCentroid(), Metric)) {
                                bestCluster[(*it)->getImageId()] = clusters[i];
                            }
                        }
                    }

                    R = R * 2.0;
                } while (curPoints - prevPoints > 0);

                /*Assign all range-searched points to their best cluster*/
                for (auto it : bestCluster) {
                    it.second->addPoint(pointsMap[it.first]);

                    if (clusterHistory.find(it.first) == clusterHistory.end() || clusterHistory[it.first]->getId() != it.second->getId()) {
                        assignments++;
                    }

                    clusterHistory[it.first] = it.second;
                }
                
                
                for(auto it : tmpPointsMap) {
                    double distanceToClosestCentroid = 1.0 / 0.0;
                    Cluster *closestCluster = NULL;
                    
                    for (unsigned int i = 0; i < clusters.size(); i++) {
                        double distance = it.second->distance(clusters[i]->getCentroid(), Metric);
                        
                        if (distance < distanceToClosestCentroid) {
                            distanceToClosestCentroid = distance;
                            closestCluster = clusters[i];
                        }
                    }
                    
                    /*Insert the current image to it's closest cluster*/
                    closestCluster->addPoint(it.second);
                    
                    if (clusterHistory.find(it.first) == clusterHistory.end() || clusterHistory[it.first]->getId() != closestCluster->getId()) {
                        assignments++;
                    }
                    
                    clusterHistory[it.first] = closestCluster;
                }
            }

            /*Update all cluster centroids*/
            if (assignments > 0) {
                for (unsigned int i = 0; i < clusters.size(); i++) {
                    clusters[i]->updateCentroid();
                }
            }

            
        } while (assignments > 100);
        double clustering_time = double(clock() - begin_clustering_time) / CLOCKS_PER_SEC;
        
        /*Print the used method*/
        outputStream << "Algorithm: ";
        if (method == "Classic") {
            outputStream << "Lloyds";
        } else if (method == "LSH") {
            outputStream << "Range Search LSH";
        } else if (method == "Hypercube") {
            outputStream << "Range Search Hypercube";
        }

        outputStream << endl;

        /*Print clustering results*/
        for (unsigned int i = 0; i < clusters.size(); i++) {
            outputStream << "CLUSTER-" << i+1 << " {size: " << clusters[i]->getSize() << ", centroid: [";
            
            for (int j = 0; j < input_dataset->getImageSize() - 1; j++) {
                outputStream << (int)clusters[i]->getCentroid()->getPixel(j) << ", ";
            }
            
            outputStream << (int)clusters[i]->getCentroid()->getPixel(input_dataset->getImageSize()-1) << "]}\n";
        }

        /*Print clustering time*/
        outputStream << "clustering_time: " << clustering_time << endl;

        /*Silhouette algorithm*/
        double averageSilhouette = 0.0;
        vector<double> s;
        
        for (unsigned int i = 0; i < input_images.size(); i++) {
            Cluster *neighbourCluster = NULL, *closestCluster = NULL;
            double distanceToClosestCentroid = 1.0/0.0, distanceToSecondClosest = 1.0/0.0;
            
            for (unsigned int j = 0; j < clusters.size(); j++) {
                double distance = input_images[i]->distance(clusters[j]->getCentroid(), Metric);

                if (distance < distanceToClosestCentroid) {
                    distanceToSecondClosest = distanceToClosestCentroid;
                    distanceToClosestCentroid = distance;

                    neighbourCluster = closestCluster;
                    closestCluster = clusters[j];
                }
                /*needed for bi*/
                else if(distance < distanceToSecondClosest) {
                    distanceToSecondClosest = distance;
                    neighbourCluster = clusters[j];
                }
            }
            
            /*Calculate average distance of ith image to images in same cluster*/
            double ai = clusterHistory[input_images[i]->getImageId()]->averageDistance(input_images[i]);
            
            /*Calculate average distance of ith image to images in the next best(neighbor) cluster*/
            double bi = neighbourCluster->averageDistance(input_images[i]);
            
            /*Calculate Silhouette score for ith image*/
            double si = (bi - ai)/max(ai, bi);
            s.push_back(si);
            averageSilhouette += si;
        }
        
        outputStream << "Silhouette: [";

        /*Calculate and print average Silhouette for each cluster*/
        for (unsigned int i = 0; i < clusters.size(); i++) {
            double averageS = 0.0;
            vector<Image*> clusterPoints = clusters[i]->getPoints();
            
            for (unsigned int j = 0; j < clusterPoints.size(); j++) {
                averageS += averageS + s[clusterPoints[j]->getImageId()];
            }
            
            outputStream << averageS/clusterPoints.size() << ",";
        }
        
        /*Print average Silhouette for all points in dataset*/
        averageSilhouette = averageSilhouette / input_images.size();
        outputStream << " " << averageSilhouette << "]\n";

        /*Print image numbers in each cluster*/
        if (complete) {
            for (unsigned int i = 0; i < clusters.size(); i++) {
                outputStream << "CLUSTER-" << i+1 << " {[";
                
                for (int j = 0; j < input_dataset->getImageSize() - 1; j++) {
                    outputStream << (int)clusters[i]->getCentroid()->getPixel(j) << ", ";
                }
                
                outputStream << (int)clusters[i]->getCentroid()->getPixel(input_dataset->getImageSize()-1) << "]";
                vector<Image*> clusterImages = clusters[i]->getPoints();
                
                for (unsigned int j = 0; j < clusterImages.size(); j++) {
                    outputStream << ", " << clusterImages[j]->getImageId();
                }
                
                outputStream << "}\n";
            }
        }

        if (method == "LSH") {
            delete lsh;
        }
        if (method == "Hypercube") {
            delete hypercube;
        }
        
        delete input_dataset;


        /* Ask user to continue or not the program*/
        string user_answer;

        while (true) {
            cout << "Do you want to continue the program with a different Query File (Yes or No)?";
            cin >> user_answer;

            if (user_answer == "Y" || user_answer == "y" || user_answer == "Yes" || user_answer == "yes") {
                cout << "Query File: ";
                cin >> input_file;
                break; 
            } 
            else if (user_answer == "N" || user_answer == "n" || user_answer == "No" || user_answer == "no") {
                cout << "Ending Program." << endl;
                continue_program = false;
                break; 
            } 
            else {
                cout << "Invalid input. Try again.\n";
            }
        }

    } 

    /* Close output_file*/
    outputStream.close();

    return 0;
}

