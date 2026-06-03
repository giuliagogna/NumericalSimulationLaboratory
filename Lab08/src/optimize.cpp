#include <iostream>
#include "compute_energy.h"

using namespace std;

int main(){

    // Initialize the random number generator
    Random rnd;
    InitializeGenerator(rnd);

    // Read Configuration
    ifstream config("inputs/config.dat");
    if (!config.is_open()) {
        cerr << "Error: Cannot open config.dat!" << endl;
        return -1;
    }

    string label;
    int n_blocks, block_length, n_eq_steps, prod_blocks, prod_block_length;
    double mu_step, sigma_step, temp, temp_step, tmin, mu, sigma;

    config >> label >> n_blocks;
    config >> label >> block_length;
    config >> label >> n_eq_steps;
    config >> label >> mu_step;
    config >> label >> sigma_step;
    config >> label >> temp;
    config >> label >> temp_step;
    config >> label >> tmin;
    config >> label >> mu;
    config >> label >> sigma;
    config >> label >> prod_blocks;
    config >> label >> prod_block_length;
    config.close();

    // Open the output files
    ofstream energy;
    string filename_energy = "energy.dat";
    OpenOutputFile(energy, filename_energy);
    energy << "# SA_step Temperature Energy Energy_error" << endl;

    ofstream trajectory;
    string filename_trajectory = "mu_sigma.dat";
    OpenOutputFile(trajectory, filename_trajectory);
    trajectory << "# SA_step Temperature Mu Sigma" << endl;

    ofstream output;
    OpenOutputFile(output, "output.dat");
    // Write configuration log
    output << "=== SIMULATED ANNEALING CONFIGURATION ===" << endl;
    output << "VMC Blocks: " << n_blocks << " | Block Length: " << block_length << endl;
    output << "Equilibration Steps: " << n_eq_steps << endl;
    output << "Initial Mu: " << mu << " | Mu Step Size: " << mu_step << endl;
    output << "Initial Sigma: " << sigma << " | Sigma Step Size: " << sigma_step << endl;
    output << "Initial Temp: " << temp << " | Temp Step: " << temp_step << endl;
    output << "=========================================\n" << endl;

    SimulatedAnnealing annealer(
        rnd,
        mu,
        sigma,
        temp,
        mu_step,
        sigma_step,
        n_blocks,
        block_length
    );

    int step = 0;

    // Variables to track the absolute minimum found during the search
    double best_energy = 1000.0;
    double best_mu = mu;
    double best_sigma = sigma;

    double initial_temperature = annealer.get_temp();

    while (temp >= tmin) {

        // Save to file
        energy << step << " " << annealer.get_temp() << " " << annealer.get_energy() << " " << annealer.get_energy_error() << endl;

        trajectory << step << " " << annealer.get_temp()<< " " << annealer.get_mu() << " " << annealer.get_sigma() << endl;

        output << "Simulation step " << step << " completed." << endl;

        // Progress that goes from 0 (start) to 1 (end)
        double progress = (initial_temperature - temp) / (initial_temperature - tmin);

        // Number of blocks goes from 5 at the highest temperature to 40 at the lowest
        int dynamic_block_number = 5 + (int)(35 * progress);

        annealer.set_n_blocks(dynamic_block_number);

        // Make the move and update temperature
        annealer.move();

        // Track the absolute best parameters
        if (annealer.get_energy() < best_energy) {
            best_energy = annealer.get_energy();
            best_mu = annealer.get_mu();
            best_sigma = annealer.get_sigma();
        }

        temp -= temp_step;
        annealer.set_temp(temp);

        // Increment step
        step++;
    }

    // Save the absolute best parameters to a file
    ofstream optimal;
    OpenOutputFile(optimal, "../inputs/optimal_params.dat");
    optimal << best_mu << endl;
    optimal << best_sigma << endl;
    optimal.close();
    
    output << "\n=== SIMULATION COMPLETED ===" << endl;
    output << "Total SA Steps: " << step << endl;
    output << "Final Optimized Parameters:" << endl;
    output << "  Mu    = " << best_mu << endl;
    output << "  Sigma = " << best_sigma << endl;
    output << "Minimum Energy: " << annealer.get_energy() << " +/- " << annealer.get_energy_error() << endl;
    output << "\nObservables saved to energy.dat and mu_sigma.dat." << endl;

    // Close the files for energy and trajectory
    energy.close();
    trajectory.close();
    output.close();

    return 0;
}