#ifndef COMPUTE_ENERGY
#define COMPUTE_ENERGY

#include <iostream>

#include "../../utils/random/random.h"
#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/metropolis/metropolis.h"
#include "../../utils/data_blocking/data_blocking.h"

using namespace std;

double energy(double x, double mu, double sigma){

    double potential_energy = pow(x, 4) - 2.5 * pow(x, 2);
    double kinetik_energy = 0.5/pow(sigma, 2) - (pow(x, 2) + pow(mu, 2))/(2.0*pow(sigma, 4)) \
                            + (x*mu)/pow(sigma, 4) * tanh((x*mu)/pow(sigma, 2));

    return potential_energy + kinetik_energy;
};

pair<double, double> hamiltonian_expectation_value(
    double mu, 
    double sigma, 
    Random& rnd,
    int n_blocks = 20, 
    int block_length=1000,
    int n_equilibration_steps=10000) {

    // Data blocking
    DataBlocker blkr(block_length);

    // Wave function and initial position
    Psi_T psi_T(mu, sigma);
    double x = 0.0; // The position in 1D

    // Metropolis: running with uniform proposal distribution as per exercise request
    MetropolisAlgorithm<double, Psi_T> metro(psi_T, x, "uniform");
    metro.tune_step(x, rnd, 1.0, false);
    metro.reset_position(x);
    metro.equilibrate(x, n_equilibration_steps, rnd, false);

    int n_mc_steps = n_blocks * block_length;

    for(int i = 0; i < n_mc_steps; i++) {
        // Perform move
        metro.perform_metropolis_move(x, rnd);
        // Measure energy and pass to the blocker for the data blocking
        blkr.add_measurement(energy(x, mu, sigma));
    }

    double mean = blkr.get_mean();
    double error = blkr.get_error();

    return make_pair(mean, error);

}

// Simulated Annealing class
class SimulatedAnnealing{
    private:

    Random& _rnd;           // Random generator
    double _mu;             // Wavefunction parameter mu
    double _sigma;          // Wavefunction parameter sigma
    double _temp;           // Temperature
    double _mu_step;        // SA step size for mu
    double _sigma_step;     // SA step size for sigma
    
    double _energy;         
    double _energy_error;

    // VMC Parameters
    int _n_blocks;
    int _block_length;

    public:
    // Constructor
    SimulatedAnnealing(
        Random& rnd, 
        double initial_mu, 
        double initial_sigma, 
        double temperature = 1.0, 
        double mu_step = 0.1, 
        double sigma_step = 0.1, 
        int n_blocks = 20, 
        int block_length = 1000) : 

        _rnd(rnd),
        _mu(initial_mu),
        _sigma(initial_sigma),
        _temp(temperature),
        _mu_step(mu_step),
        _sigma_step(sigma_step),
        _n_blocks(n_blocks),
        _block_length(block_length) 

        {
            // Calculates the initial energy inside
            pair<double, double> init_res = hamiltonian_expectation_value(_mu, _sigma, _rnd, _n_blocks, _block_length);
            _energy = init_res.first;
            _energy_error = init_res.second;
        }

    // Getters
    double get_temp() {return _temp;}
    double get_mu() {return _mu;}
    double get_sigma() {return _sigma;}
    double get_energy() {return _energy;}
    double get_energy_error() {return _energy_error;}

    // Setter needed by main to update temperature
    void set_temp(double new_temp) {_temp = new_temp;}

    // Setter needed to make the number of blocks in data blocking dynamic
    void set_n_blocks(int n_blocks){_n_blocks = n_blocks;}

    // Method that performs a move at temperature _temp
    void move(){

        // Propose new point in the parameter space
        double new_mu = _mu + (_rnd.Rannyu() - 0.5) * _mu_step;
        double new_sigma = _sigma + (_rnd.Rannyu() - 0.5) * _sigma_step;
        
        // Reject the move if the values are negative: avoid degeneration
        if (new_mu <= 0.0 || new_sigma <= 0.0) {
            return; 
        }

        // Evaluate energy in the new configuration
        pair<double, double> result = hamiltonian_expectation_value(new_mu, new_sigma, _rnd, _n_blocks, _block_length);

        double new_energy = result.first;
        double new_error = result.second;

        double p = 0.0;

        if (new_energy < _energy){
            p = 1.0;
        } else {
            p = exp((_energy - new_energy)/_temp);
        }

        // Extract and decide wether or not to accept
        if (_rnd.Rannyu() < p) {
            _mu = new_mu;
            _sigma = new_sigma;
            _energy = new_energy;
            _energy_error = new_error;
        }

    };

};


#endif // COMPUTE_ENERGY