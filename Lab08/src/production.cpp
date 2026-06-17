#include <iostream>
#include <fstream>
#include <string>
#include "simulated_annealing.h"

using namespace std;

int main(){
    Random rnd;
    InitializeGenerator(rnd);

    // Read Optimal Parameters from file
    ifstream optimal("inputs/optimal_params.dat");
    if (!optimal.is_open()) {
        cerr << "Error: Run optimize.exe first!" << endl;
        return -1;
    }

    double best_mu, best_sigma;
    optimal >> best_mu >> best_sigma;
    optimal.close();

    // Read the parameters we need from the configuration file
    ifstream config("inputs/config.dat");
    string label;
    int n_eq_steps, prod_blocks, prod_block_length;

    // skip the lines until we reach the ones we need
    for(int i=0; i<3; i++) config >> label >> label; 
    config >> label >> n_eq_steps;
    for(int i=0; i<7; i++) config >> label >> label; 
    config >> label >> prod_blocks;
    config >> label >> prod_block_length;
    config.close();

    // Set up files for output
    ofstream best_energy_file("outputs/best_energy_blocks.dat");
    best_energy_file << "# Block Mean Error" << endl;

    ofstream x_hist_file("outputs/sampled_x.dat");
    x_hist_file << "# x" << endl;

    cout << "Starting Production Run with Mu=" << best_mu << ", Sigma=" << best_sigma << endl;
    cout << "Blocks: " << prod_blocks << ", Steps/Block: " << prod_block_length << endl;


    // Initialization
    DataBlocker prod_blocker(prod_block_length);
    Psi_T optimal_psi(best_mu, best_sigma);
    double x = 0.0;

    MetropolisAlgorithm<double, Psi_T> prod_metro(optimal_psi, x, "uniform");
    prod_metro.tune_step(x, rnd, 1.0, true); 
    prod_metro.equilibrate(x, n_eq_steps, rnd, true);

    int total_steps = prod_blocks * prod_block_length;

    for (int i = 0; i < total_steps; i++) {
        prod_metro.perform_metropolis_move(x, rnd);
        
        x_hist_file << x << "\n";
        
        double e_loc = energy(x, best_mu, best_sigma);
        prod_blocker.add_measurement(e_loc);

        // Print on file only when a block has finished
        if ((i+1)%prod_block_length==0 && i !=0) {
            best_energy_file << prod_blocker.get_completed_blocks() << " " 
                             << prod_blocker.get_mean() << " " 
                             << prod_blocker.get_error() << "\n";
        }
    }

    best_energy_file.close();
    x_hist_file.close();
    
    cout << "Production run complete. Final Energy: " << prod_blocker.get_mean() 
         << " +/- " << prod_blocker.get_error() << endl;

    return 0;
}