#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>

#include "include/Dataset.h"
#include "include/BruteforceSearch.h"
#include "include/HypercubeFunction.h"
#include "include/InputHandling.h"

using namespace std;

/* Number of images to take from the Query File */
#define QUERY_IMAGES 50

/* Window (usually [2, 6]) */
#define W 6


/* main function */
int main(int argc, char *argv[]) {

    /* Variables */
    bool continue_program = true;
    chrono::milliseconds sum_cube(0);
    chrono::milliseconds sum_bruteforce(0);

    /* Variables for arguments */
    string input_file, query_file, output_file;
	int k = 14;         // num of LSH functions
    int M = 10;         // max num of candidate points to be checked
    int probes = 2;     // max num of vertices of the hypercube to be checked
    int N = 1;          // num of ANN
    double R = 1000;    // search ray

    /* Check the arguments that the user gave */
    if (check_args(argc, argv, "hupercube") == -1) {
        return 0;
    }
    /* Parse the arguments from the user */
    else if (Hypercube_get_input(argc, argv, &input_file, &query_file, &k, &M, &probes, &output_file, &N, &R) == -1) {
        return 0;
    }

    cout << "-------------------------------------------" << endl;
    cout << "Input File: " << input_file << endl;
    cout << "Query File: " << query_file << endl;
    cout << "Output File: " << output_file << endl;
    cout << "k: " << k << endl;
    cout << "M: " << M << endl;
    cout << "probes: " << probes << endl;
    cout << "N: " << N << endl;
    cout << "R: " << R << endl << endl;


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

    /* Get all images from input_dataset */ 
    vector<Image*> input_images = input_dataset->getImages();


    /* Open Output File */ 
    ofstream outputStream(output_file);
    if (!outputStream) {
        cout << "An error occurred while opening file: " << output_file << endl;
    }


    /* Initialize Hypercube interface */
    Hypercube *hypercube = new Hypercube(input_dataset, k, W);
    
    /* Initialize Bruteforce interface */
    Bruteforce *bruteforce = new Bruteforce(input_images);


    outputStream << "--------------- HYPERCUBE ---------------" << endl << endl;

    /* Run this loop until the user stop it */
    while (continue_program == true) {
        /* Initialize the dataset from the Query File */
        Dataset *query_dataset = new Dataset(query_file);

        if (!query_dataset->isValid()) {
            cout << "Invalid dataset: " << input_file << endl;
            delete query_dataset;
            break;
        }

        /* Print out the images in the query_dataset if you want */ 
        // query_dataset->printImages();


        /* Get all images from input_dataset */ 
        vector<Image*> query_images = query_dataset->getNumImages(QUERY_IMAGES);

        /* Check if the query_images were taken correctly */
        if (query_images.empty()) {
            return 0;
        }

        cout << QUERY_IMAGES << " images were taken for: " << input_file << endl << endl;

        /* Print out the query_images if you want */ 
        // queryset->printselectedImages(query_images);

        /* Run Hypercube and Bruteforce for every query image*/
        for (unsigned int i = 0; i < query_images.size(); i++) {
            outputStream << "Query: " << query_images[i]->getImageId() << endl;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /* Count the time the Bruteforce takes*/
            auto start_bruteforce = chrono::high_resolution_clock::now();
            vector<pair<double, int>> exactNearestNeighbours = bruteforce->exactNN(query_images[i],N);
            auto end_bruteforce = chrono::high_resolution_clock::now();

            /* Total time of one query for Bruteforce */
            auto bruteforce_time = chrono::duration_cast<chrono::milliseconds>(end_bruteforce - start_bruteforce).count();

            /* Total time of all queries for Bruteforce */
            sum_bruteforce += chrono::milliseconds(bruteforce_time);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /* Count the time the Hypercube takes*/
            auto start_cube = chrono::high_resolution_clock::now();
            list<pair<double, int>> hypercubeNeighbours = hypercube->searchSimilarPoints(query_images[i],M,probes);
            hypercubeNeighbours.sort();
            auto end_cube = chrono::high_resolution_clock::now();
            hypercubeNeighbours.resize(N);

            /* Total time of one query for LSH */
            auto cube_time = chrono::duration_cast<chrono::milliseconds>(end_cube - start_cube).count();

            /* Total time of all queries for LSH */
            sum_cube += chrono::milliseconds(cube_time);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            unsigned int j = 0;
            for (list<pair<double, int>>::iterator it = hypercubeNeighbours.begin(); it != hypercubeNeighbours.end(); it++) {
                outputStream << "Nearest neighbor-" << j + 1 << ": " << it->second << endl;
                outputStream << "distanceHypercube: " << it->first << endl;
                outputStream << "distanceTrue: " << exactNearestNeighbours[j++].first << endl;
            }

            outputStream << "tHypercube: " << cube_time << endl;
            outputStream << "tTrue: " << bruteforce_time << endl;
            outputStream << "R-near neighbors:\n";

            /* Nearest Neighbours in Range R */
            list<Image*> rNN = hypercube->rangeSearch(query_images[i], M, probes, R);
            if (rNN.size() != 0) {
                for (list<Image*>::iterator it = rNN.begin(); it != rNN.end(); it++) {
                    outputStream << (*it)->getImageId() << endl;
                }
            } 
            else {
                outputStream << "None Nearest Neighbours in Range R were found\n";
            }

            outputStream << endl;

            cout << "Query: " << query_images[i]->getImageId() << " checked." << endl;
        }

        cout << "-------------------------------------------" << endl;

        cout << "Total LSH execution time: " << sum_cube.count() << " milliseconds" << endl;
        cout << "Total LSH execution time: " << sum_bruteforce.count() << " milliseconds" << endl << endl;
        cout << "Check " << output_file << " for the results" << endl << endl;

        delete query_dataset;

        /* Ask user to continue or not the program*/
        string user_answer;

        while (true) {
            cout << "Do you want to continue the program with a different Query File (Yes or No)?";
            cin >> user_answer;

            if (user_answer == "Y" || user_answer == "y" || user_answer == "Yes" || user_answer == "yes") {
                cout << "Query File: ";
                cin >> query_file;
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

    /* Close output_file */
    outputStream.close();


    delete bruteforce;
    delete hypercube;
    delete input_dataset;


    return 0;
}