#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "../random/random.h"
#include <fstream>
#include <string>

using namespace std;

// This function initializes the random generator
void InitializeGenerator(Random &rnd);

// Random generator initializer for Parallel GA
void InitializeMPI_Generator(Random& rnd, int rank);

// This function opens an output file and checks if it was opened successfully
void OpenOutputFile(ofstream& output_file, const string& filename);

//This function computes the statistica uncertainty
double error(double AV, double AV2, int n);

// This function calculates the current number of throws up to the current block
int current_throws(int i, int L);

#endif