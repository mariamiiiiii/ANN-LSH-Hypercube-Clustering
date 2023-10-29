# LSH, Hypercube and Cluster for the MNIST Dataset

In this project, the following algorithms were implemented: 
* Approximate Nearest Neighbour Algorithms LSH for d-dimensional vectors based on the Euclidean metric 
* Randomized projection onto the hypercube with the Euclidean metric
* Clustering (Classic, LSH, Hypercube)
Μore specifically in this project, the MNIST dataset was used, which contains images.

## Compilation and Run
For LSH
```
make lsh
make run_lsh or ./build/lsh -d dataset/input_file -q dataset/query_file -k 4 -L 5 -o dataset/output_file -N 1 -R 10000
```
For Hypercube
```
make cube
make run_cube or ./build/cube -d dataset/input_file -q dataset/query_file -k 14 -M 10 -probes 2 -o dataset/output_file -N 1 -R 10000
```

## Clean 
```
make clean
```


## Contributors:
1. [lydia Kyriakou](https://github.com/LydiaKyriakou)
2. [Maria Michailidou](https://github.com/mariamiiiiii)