#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "functions.h"
#include "../random/random.h"

using namespace std;

// This function initializes the random number generator
void InitializeGenerator(Random &rnd){
    int seed[4];
    int p1, p2;
    ifstream Primes("../utils/random/Primes");
    if (Primes.is_open()){
       Primes >> p1 >> p2 ;
//       Primes >> p1 >> p2 ;
//       Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();
 
    ifstream input("../utils/random/seed.in");
    string property;
    if (input.is_open()){
       while ( !input.eof() ){
          input >> property;
          if( property == "RANDOMSEED" ){
             input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
             rnd.SetRandom(seed,p1,p2);
          }
       }
       input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;
}

// This function opens an output file in the "outputs" directory, creating the directory if it does not exist
void OpenOutputFile(ofstream& output_file, const string& filename) {

    // If there is no "outputs" directory, create it to store the output files
    if (!std::filesystem::exists("outputs")) {
        std::filesystem::create_directory("outputs");
    }
    
    string filepath = "outputs/" + string(filename);
    
    output_file.open(filepath);
    
    if (!output_file.is_open()) {
        cerr << "PROBLEM: Unable to create or open file: " << filepath << endl;
        exit(1); 
    }
}



// This function computes the statistical uncertainty
// The function is suitable to be used in progressive calculations
double error(double AV, double AV2, int n){
    if (n==0){
        return 0.0;
    } else {
        return sqrt((AV2 - AV*AV)/n);
    }
}

// This function computes the number of throws at the i-th block, given the total number of throws L
int current_throws(int i, int L){
    return (i + 1) * L;
}