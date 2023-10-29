#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <chrono>

#include "include/InputHandling.h"
#include "include/Dataset.h"
#include "include/lshFunctions.h"
#include "include/BruteforceSearch.h"

using namespace std;

/* Number of images to take from the Query File */
#define QUERY_IMAGES 50

/* Window (usually [2, 6]) */
#define W 6


/* main function */
int main(int argc, char *argv[]) {

    /* Variables */
    bool continue_program = true;
    chrono::milliseconds sum_lsh(0);
    chrono::milliseconds sum_bruteforce(0);

    /* Variables for arguments */
    string input_file, query_file, output_file;
	int k = 4;         // num of LSH functions
    int L = 5;         // num of hash tables
    int N = 1;         // num of ANN
    double R = 1000;   // search ray


    /* Check the arguments that the user gave */
    if (check_args(argc, argv, "lsh") == -1) {
        return 0;
    }
    /* Parse the arguments from the user */
    else if (LSH_get_input(argc, argv, &input_file, &query_file, &k, &L, &output_file, &N, &R) == -1) {
        return 0;
    }

    cout << "-------------------------------------------" << endl;
    cout << "Input File: " << input_file << endl;
    cout << "Query File: " << query_file << endl;
    cout << "Output File: " << output_file << endl;
    cout << "k: " << k << endl;
    cout << "L: " << L << endl;
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


    /* Initialize LSH interface */
    LSH *lsh = new LSH(k, W, L, input_dataset);

    /* Initialize Bruteforce interface */
    Bruteforce *bruteforce = new Bruteforce(input_images);


    outputStream << "--------------- LSH ---------------" << endl << endl;

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

        /* Get some images from input_dataset */ 
        vector<Image*> query_images = query_dataset->getNumImages(QUERY_IMAGES);

        /* Check if the query_images were taken correctly */
        if (query_images.empty()) {
            return 0;
        }

        cout << QUERY_IMAGES << " images were taken for: " << input_file << endl << endl;

        /* Print out the query_images if you want */ 
        // queryset->printselectedImages(query_images);

        /* Run LSH and Bruteforce for every query image*/
        for (int i = 0; i < query_images.size(); i++) {
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

            /* Count the time the LSH takes*/
            auto start_lsh = chrono::high_resolution_clock::now();
            vector<pair<double, int>> lshNearestNeighbours = lsh->approximate_kNN(query_images[i], N);
            auto end_lsh = chrono::high_resolution_clock::now();

            /* Total time of one query for LSH */
            auto lsh_time = chrono::duration_cast<chrono::milliseconds>(end_lsh - start_lsh).count();

            /* Total time of all queries for LSH */
            sum_lsh += chrono::milliseconds(lsh_time);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /* For every N - Nearest Neighbour*/
            for (int n = 0; n < lshNearestNeighbours.size(); n++) {
                outputStream << "Nearest neighbor-" << n + 1 << ": " << lshNearestNeighbours[n].second << endl;
                outputStream << "distanceLSH: " << lshNearestNeighbours[n].first << endl;
                outputStream << "distanceTrue: " << exactNearestNeighbours[n].first << endl;
            }

            outputStream << "tLSH: " << lsh_time << endl;
            outputStream << "tTrue: " << bruteforce_time << endl;
            outputStream << "R-near neighbors:\n";

            /* Nearest Neighbours in Range R */
            vector<Image*> rANN = lsh->rangeSearch(query_images[i], R);


            if (rANN.size() != 0) {
                /* Nearest Neighbours in Range R */
                for (int j = 0; j < rANN.size(); j++) {
                    outputStream << rANN[j]->getImageId() << endl;
                }
            } 
            else {
                outputStream << "None Nearest Neighbours in Range R were found.\n";
            }
            
            outputStream << endl;

            cout << "Query: " << query_images[i]->getImageId() << " checked." << endl;
        }

        cout << "-------------------------------------------" << endl;

        cout << "Total LSH execution time: " << sum_lsh.count() << " milliseconds" << endl;
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


    delete lsh;
    delete bruteforce;
    delete input_dataset;


    return 0;
}




