#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/random/random.h"
#include "../../utils/RWlib/RWlib.h"

#include <iostream>
#include <vector>


using namespace std;

int main(){

    Random rnd;
    InitializeGenerator(rnd);

    ofstream output;
    OpenOutputFile(output, "02_2_RW.dat");
    output << "#step r_squared_lattice error_lattice r_squared_continuous error_continuous" << endl;

    int M = 100000;              // Total number of random walks
    int N = 100;                // Number of blocks for averaging 
    int L = M / N;              // Number of random walks in each block
    int n_steps = 100;          // Number of steps in each random walk 
    float step_length = 1.0;    // Length of each step in the random walk

    // Vectors to store the progressive averages and their squares for error calculation
    vector<double> prog_lattice(n_steps, 0.0);
    vector<double> prog_lattice_squared(n_steps, 0.0);
    vector<double> prog_continuous(n_steps, 0.0);
    vector<double> prog_continuous_squared(n_steps, 0.0);

    for (int i = 0; i < N; i++){

        // Set every element of the block sum vectors to zero at the beginning of each block
        vector<double> block_sum_lattice(n_steps, 0.0);
        vector<double> block_sum_continuous(n_steps, 0.0);

        for (int j = 0; j < L; j++){
            // Define starting position for both lattice and continuous random walks
            Position p_lat = {0.0, 0.0, 0.0}; 
            Position p_cont = {0.0, 0.0, 0.0};

            for(int step = 0; step < n_steps; step++){
                // Make a step in the lattice
                block_sum_lattice[step] += (p_lat.x*p_lat.x + p_lat.y*p_lat.y + p_lat.z*p_lat.z);
                step_lattice(p_lat, rnd, step_length);

                // Make a step in continuous space
                block_sum_continuous[step] += (p_cont.x*p_cont.x + p_cont.y*p_cont.y + p_cont.z*p_cont.z);
                step_continuum(p_cont, rnd, step_length);
            }
        }

        // At the end of the block, compute the root mean square for both lattice and continuous cases, and update the progressive averages and their squares for error calculation
        for (int step = 0; step < n_steps; step++){
            
            double r_rms_lat = sqrt(block_sum_lattice[step] / L);
            double r_rms_cont = sqrt(block_sum_continuous[step] / L);

            prog_lattice[step] = (double(i)/(i+1)) * prog_lattice[step] + r_rms_lat/(i+1);
            prog_lattice_squared[step] = (double(i)/(i+1)) * prog_lattice_squared[step] + (r_rms_lat * r_rms_lat)/(i+1);

            prog_continuous[step] = (double(i)/(i+1)) * prog_continuous[step] + r_rms_cont/(i+1);
            prog_continuous_squared[step] = (double(i)/(i+1)) * prog_continuous_squared[step] + (r_rms_cont * r_rms_cont)/(i+1);
        }
    }

    // At the end of all blocks, output the results to a file
    // Format: Step | root_mean_squared_lattice | err_lattice | root_mean_squared_continuous | error_continuous
    for (int step = 0; step < n_steps; step++){
        output << step << " " 
               << prog_lattice[step] << " " 
               << error(prog_lattice[step], prog_lattice_squared[step], N-1) << " "
               << prog_continuous[step] << " " 
               << error(prog_continuous[step], prog_continuous_squared[step], N-1) << endl;
    }


    output.close();

    return 0;
}