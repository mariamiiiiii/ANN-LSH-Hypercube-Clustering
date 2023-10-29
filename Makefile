# Compiler
CC = g++

# Paths
MODULES = ./modules/
INCLUDE = ./include/
BUILD = ./build/

# Arguments
ARG_LSH = -d dataset/input_file -q dataset/query_file -k 4 -L 5 -o dataset/output_file -N 1 -R 10000
ARG_CUBE = -d dataset/input_file -q dataset/query_file -k 14 -M 10 -probes 2 -o dataset/output_file -N 1 -R 10000
ARG_CLUSTER = -i dataset/query_file -c cluster.conf -o dataset/output_file -complete 0 -m LSH


LSH_OBJS = $(BUILD)lsh.o $(BUILD)InputHandling.o $(BUILD)Dataset.o $(BUILD)Image.o $(BUILD)HashFunctions.o $(BUILD)HashTable.o $(BUILD)lshFunctions.o $(BUILD)BruteforceSearch.o
CUBE_OBJS = $(BUILD)hypercube.o $(BUILD)HypercubeFunction.o $(BUILD)InputHandling.o $(BUILD)Dataset.o $(BUILD)Image.o $(BUILD)HashFunctions.o $(BUILD)HashTable.o $(BUILD)lshFunctions.o $(BUILD)BruteforceSearch.o
CLUSTER_OBJS = $(BUILD)cluster.o $(BUILD)InputHandling.o $(BUILD)ClusterFunction.o $(BUILD)Dataset.o $(BUILD)Image.o $(BUILD)HashFunctions.o $(BUILD)HashTable.o $(BUILD)lshFunctions.o $(BUILD)BruteforceSearch.o $(BUILD)HypercubeFunction.o

# Compiled
lsh: $(LSH_OBJS)
	$(CC) -o $(BUILD)lsh $(LSH_OBJS)  

cube: $(CUBE_OBJS)
	$(CC) -o $(BUILD)cube $(CUBE_OBJS) 

cluster: $(CLUSTER_OBJS)
	$(CC) -o $(BUILD)cluster $(CLUSTER_OBJS) 


$(BUILD)lsh.o: lsh.cpp
	@echo " Compile lsh ...";
	$(CC) -c -o $(BUILD)lsh.o -I$(INCLUDE) lsh.cpp 

$(BUILD)hypercube.o: hypercube.cpp
	@echo " Compile hypercube ...";
	$(CC) -c -o $(BUILD)hypercube.o -I$(INCLUDE) hypercube.cpp

$(BUILD)cluster.o: cluster.cpp
	@echo " Compile cluster ...";
	$(CC) -c -o $(BUILD)cluster.o -I$(INCLUDE) cluster.cpp 

$(BUILD)ClusterFunction.o: $(MODULES)ClusterFunction.cpp
	@echo " Compile ClusterFunction ...";
	$(CC) -c -o $(BUILD)ClusterFunction.o -I$(INCLUDE) $(MODULES)ClusterFunction.cpp 

$(BUILD)HypercubeFunction.o: $(MODULES)HypercubeFunction.cpp
	@echo " Compile HypercubeFunction ...";
	$(CC) -c -o $(BUILD)HypercubeFunction.o -I$(INCLUDE) $(MODULES)HypercubeFunction.cpp 

$(BUILD)InputHandling.o: $(MODULES)InputHandling.cpp
	@echo " Compile InputHandling ...";
	$(CC) -c -o $(BUILD)InputHandling.o -I$(INCLUDE) $(MODULES)InputHandling.cpp 

$(BUILD)Dataset.o: $(MODULES)Dataset.cpp
	@echo " Compile Dataset ...";
	$(CC) -c -o $(BUILD)Dataset.o -I$(INCLUDE) $(MODULES)Dataset.cpp 

$(BUILD)Image.o: $(MODULES)Image.cpp
	@echo " Compile Image ...";
	$(CC) -c -o $(BUILD)Image.o -I$(INCLUDE) $(MODULES)Image.cpp 

$(BUILD)HashFunctions.o: $(MODULES)HashFunctions.cpp
	@echo " Compile HashFunctions ...";
	$(CC) -c -o $(BUILD)HashFunctions.o -I$(INCLUDE) $(MODULES)HashFunctions.cpp 

$(BUILD)HashTable.o: $(MODULES)HashTable.cpp
	@echo " Compile HashTable ...";
	$(CC) -c -o $(BUILD)HashTable.o -I$(INCLUDE) $(MODULES)HashTable.cpp 

$(BUILD)lshFunctions.o: $(MODULES)lshFunctions.cpp
	@echo " Compile lshFunctions ...";
	$(CC) -c -o $(BUILD)lshFunctions.o -I$(INCLUDE) $(MODULES)lshFunctions.cpp 

$(BUILD)BruteforceSearch.o: $(MODULES)BruteforceSearch.cpp
	@echo " Compile BruteforceSearch ...";
	$(CC) -c -o $(BUILD)BruteforceSearch.o -I$(INCLUDE) $(MODULES)BruteforceSearch.cpp 

# Run
run_lsh:
	@echo "Running lsh ..."
	$(BUILD)lsh $(ARG_LSH)

run_cube:
	@echo "Running cube ..."
	$(BUILD)cube $(ARG_CUBE)

run_cluster:
	@echo "Running cluster ..."
	$(BUILD)cluster $(ARG_CLUSTER)


# Clean
clean:
	@echo "Clean ..."
	rm -f $(BUILD)lsh $(LSH_OBJS) $(BUILD)cube $(CUBE_OBJS) $(BUILD)cluster $(CLUSTER_OBJS)