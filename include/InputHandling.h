#include "HypercubeFunction.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include <unistd.h>

using namespace std;


/* Function Declarations */
int check_args(int argc, char *argv[], string method);
int LSH_get_input(int argc, char *argv[], string* input_file, string* query_file, int* k, int* L, string* output_file, int* N, double* R);
int Hypercube_get_input(int argc, char *argv[], string* input_file, string* query_file, int* k, int* M, int* probes, string* output_file, int* N, double* R);
int cluster_get_input(int argc, char *argv[], string* input_file, string* configuration_file, string* output_cluster_file, int* complete, string* method);


int get_input2(string* input_file, string* configuration_file, string* output_cluster_file, int* complete, string* method, int argc, char *argv[]);
int check_args2(int argc, char *argv[]);