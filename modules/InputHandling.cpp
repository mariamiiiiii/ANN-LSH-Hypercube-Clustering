#include "../include/InputHandling.h"

/* Return -1 if args are not in pairs or greater than they should be */
int check_args(int argc, char *argv[], string method) {
	if (method == "lsh") {
		string usage = "USAddddGE: $./lsh -d <input file> -q <query file> -k <int> -L <int> -o <output file> -N <number of nearest> -R <radius>";
		if (argc > 15 || (argc%2 != 1) ){
			cerr << "Wrong number of arguments: " + usage << endl;
			return -1;
		}
	}
	else if (method == "hupercube") {
		string usage = "USAGE: $./cube -d <input file> -q <query file> -k <int> -M <int> -probes <int> -o <output file> -N <number of nearest> -R <radius>";
		if (argc > 17 || (argc%2 != 1) ){
			cerr << "Wrong number of arguments: " + usage << endl;
			return -1;
		}
	}
	else if (method == "cluster_get_input") {
		string usage = "Usage: $./cluster  -i  <input  file>  -c  <configuration  file>  -o  <output  file>  -complete  <optional> -m <method: Classic OR LSH or Hypercube>";
		if (argc > 11 || (argc%2 != 1) ){
			cerr << "Wrong number of arguments: " + usage << endl;
			return -1;
		}
	}

    return 1;
}


int LSH_get_input(int argc, char *argv[], string* input_file, string* query_file, int* k, int* L, string* output_file, int* N, double* R) {
    string usage = "USAGE: $./lsh -d <input file> -q <query file> -k <int> -L <int> -o <output file> -N <number of nearest> -R <radius>\n";

    int opt;

    /* getopt will return the next option character found in argv. If there are no more options, it returns -1. */
	while ((opt = getopt(argc, argv, "d:q:k:L:o:N:R:")) != -1) { 
		switch(opt){
			case 'd':
				*input_file = optarg;
				break;
			case 'q':
				*query_file = optarg;
				break;
			case 'k':
				*k = atoi(optarg);
				break;			
			case 'L':
				*L = atoi(optarg);
				break;
			case 'o':
				*output_file = optarg; 
				break;
            case 'N':
				*N = atoi(optarg);
				break;
            case 'R':
				*R = atoi(optarg);
				break;
			default:
				cerr << usage ;
				return -1;
		}
	}
	if ( input_file->empty() ){
		cout << "Please enter path for input file" << endl;
		cin >> *input_file; 
	}
	if ( query_file->empty() ){
		cout << "Please enter path for query file" << endl;
		cin >> *query_file;
	}
	if ( output_file->empty() ){
		cout << "Please enter path for output file" << endl;
		cin >> *output_file;
	}

    
    return 0;

}


int Hypercube_get_input(int argc, char *argv[], string* input_file, string* query_file, int* k, int* M, int* probes, string* output_file, int* N, double* R) {
    string usage = "USAGE: $./cube -d <input file> -q <query file> -k <int> -M <int> -probes <int> -o <output file> -N <number of nearest> -R <radius>";

	for (int i = 1; i < argc; ++i) {
        // Check if the argument is "-probes"
        if (strcmp(argv[i], "-probes") == 0) {
            // Replace "-probes" with "-p"
            argv[i] = const_cast<char*>("-p");
        }
    }

    int opt;

    /* getopt will return the next option character found in argv. If there are no more options, it returns -1. */
	while ((opt = getopt(argc, argv, "d:q:k:M:p:o:N:R:")) != -1) { 
		switch(opt){
			case 'd':
				*input_file = optarg;
				break;
			case 'q':
				*query_file = optarg;
				break;
			case 'k':
				*k = atoi(optarg);
				break;			
			case 'M':
				*M = atoi(optarg);
				break;
			case 'p':
				*probes = atoi(optarg);
				break;
			case 'o':
				*output_file = optarg; 
				break;
            case 'N':
				*N = atoi(optarg);
				break;
            case 'R':
				*R = atoi(optarg);
				break;
			default:
				cerr << usage << endl;
				return -1;
		}
	}
	if ( input_file->empty() ){
		cout << "Please enter path for input file" << endl;
		cin >> *input_file; 
	}
	if ( query_file->empty() ){
		cout << "Please enter path for query file" << endl;
		cin >> *query_file;
	}
	if ( output_file->empty() ){
		cout << "Please enter path for output file" << endl;
		cin >> *output_file;
	}

    return 0;

}


int cluster_get_input(int argc, char *argv[], string* input_file, string* configuration_file, string* output_cluster_file, int* complete, string* method) {
    string usage = "USAGE:$ ./cluster -i  <input  file>  -c  <configuration  file>  -o  <output  file>  -complete  <optional> -m <method: Classic OR LSH or Hypercube>";

	for (int i = 1; i < argc; ++i) {
        // Check if the argument is "-probes"
        if (strcmp(argv[i], "-complete") == 0) {
            // Replace "-probes" with "-p"
            argv[i] = const_cast<char*>("-e");
        }
    }

    int opt;

    /* getopt will return the next option character found in argv. If there are no more options, it returns -1. */
	while ((opt = getopt(argc, argv, "i:c:o:e:m:")) != -1) { 
		switch(opt){
			case 'i':
				*input_file = optarg;
				break;
			case 'c':
				*configuration_file = optarg;
				break;
			case 'o':
				*output_cluster_file = optarg;
				break;			
			case 'e':
				*complete = atoi(optarg);
				break;
			case 'm':
				*method = optarg;
				break;
			default:
				cerr << usage << endl;
				return -1;
		}
	}
	if ( input_file->empty() ){
		cout << "Please enter path for input file" << endl;
		cin >> *input_file; 
	}
	if ( configuration_file->empty() ){
		cout << "Please enter path for query file" << endl;
		cin >> *configuration_file;
	}
	if ( output_cluster_file->empty() ){
		cout << "Please enter path for output file" << endl;
		cin >> *output_cluster_file;
	}
    
    return 0;

}








/* Return -1 if args are not in pairs or greater that 15 */
int check_args2(int argc, char *argv[]) {
    string s = "Usage:$./cluster  –i  <input  file>  –c  <configuration  file>  -o  <output  file>  -complete  <optional> -m <method: Classic OR LSH or Hypercube>\n";
	if (argc>15 || (argc%2 != 1) ){
		cerr << "Wrong number of arguments: " + s ;
		return -1;
	}

    return 1;
}


int get_input2(string* input_file, string* configuration_file, string* output_cluster_file, int* complete, string* method, int argc, char *argv[]) {
    string usage = "USAGE:$./cluster  –i  <input  file>  –c  <configuration  file>  -o  <output  file>  -b  <optional> -m <method: Classic OR LSH or Hypercube>\n";

    int opt;

    /* getopt will return the next option character found in argv. If there are no more options, it returns -1. */
	while ((opt = getopt(argc, argv, "i:c:o:b:m:")) != -1) { 
		switch(opt){
			case 'i':
				*input_file = optarg;
				break;
			case 'c':
				*configuration_file = optarg;
				break;
			case 'o':
				*output_cluster_file = optarg;
				break;			
			case 'b':
				*complete = atoi(optarg);
				break;
			case 'm':
				 *method = optarg;
				break;
			default:
				cerr << usage ;
				return -1;
		}
	}
	/*if( *k <= 0 || *L <= 0){
		cerr << "Arguments k and L should be > 0 : " + usage ;
		return -1;
	}*/
	if ( input_file->empty() ){
		cout << "Please enter path for input file\n";
		cin >> *input_file; 
	}
	if ( configuration_file->empty() ){
		cout << "Please enter path for query file\n";
		cin >> *configuration_file;
	}
	if ( output_cluster_file->empty() ){
		cout << "Please enter path for output file\n";
		cin >> *output_cluster_file;
	}


    // cout << "Input File: " << *input_file << endl;
    // cout << "Query File: " << *query_file << endl;
    // cout << "k: " << *k << endl;
    // cout << "L: " << *L << endl;
    // cout << "Output File: " << *output_file << endl;
    // cout << "N: " << *N << endl;
    // cout << "R: " << *R << endl;
    
    return 0;

}
