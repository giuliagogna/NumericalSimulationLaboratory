#include <iostream>
#include "compute_energy.h"

using namespace std;

int main(){

    // Initialize the random number generator
    Random rnd;
    InitializeGenerator(rnd);

    // Configuration
    int n_blocks = 20;
    int blok_length = 1000;
    int n_equilibration_steps = 10000;

    // Starting values for mu and sigma
    double mu=0.0; double sigma=1.0;

    return 0;
}