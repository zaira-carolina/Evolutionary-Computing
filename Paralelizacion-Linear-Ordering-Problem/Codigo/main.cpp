/*******************************************************************************************
Authors: Carlos Segura (Programmer and Designer), Lazaro Lugo (Programmer and Designer),
Gara Miranda (Designer)

Description: main function to execute MA-EDM to an instance.
Parameters:
- Number of individuals in the population
- Crossover: it can be cx or ob
- Stopping criterion in time
- Seed
- Instance file
- Output file
********************************************************************************************/


#include "MA.h"
#include <unistd.h>
#include <bits/stdc++.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <mpi.h>

Problem *Individual::problem;
int main(int argc, char **argv){

	MPI_Init(&argc, &argv);

	if(argc!=7){
		cout << "Error. Usage: " << argv[0] << " N crossover stopping_criterion seed instance_file output_file" << endl; 
		exit(0);
	}
	int N = atoi(argv[1]);
	double pc = 1.0;
	string crossType = argv[2];
	double finalTime = atof(argv[3]);
	int seed = atoi(argv[4]);
	string instanceFile = argv[5];
	string outputFile = argv[6];

	srand(seed);

	int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	MA ma(N, pc, crossType, finalTime, outputFile);
	Problem p(instanceFile);
	Individual::problem = &p;
	
	if (world_rank == 0) {
        // Master process
        ma.run();
    } else {
		// Slave process
		ma.intensifySlave();

	}
	
	MPI_Finalize(); 
}
