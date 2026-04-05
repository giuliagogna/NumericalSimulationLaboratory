#ifndef METROPOLIS_H
#define METROPOLIS_H

#include <iostream>
#include <cmath>
#include <string>

#include "../random/random.h"
#include "../auxiliary_functions/functions.h"

// Position in 3D space
struct Position {
    double x;
    double y;
    double z;
};

// ==============================================================
// DISTRIBUTIONS TO SAMPLE
// ==============================================================

// Note: avoid all normalization constants, since they cancel in the acceptance ratio

// Hydrogen ground state wavefunction (1s orbital) (squared module)
struct Psi100{
    double a0 = 1.0; // Bohr radius in atomic units
    double operator()(Position p) const {
        double r = std::sqrt(p.x*p.x + p.y*p.y + p.z*p.z);

        // Return |Psi|^2 proportional to exp(-2r/a0)
        return std::exp(-2.0 * r / a0);
    }
};

// Hydrogen 2p orbital wavefunction (n=2, l=1, m=0) (squared module)
struct Psi210{
    double a0 = 1.0; // Bohr radius in atomic units
    double operator()(Position p) const {
        double r = std::sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
        double z = p.z;

        // |Psi|^2 is proportional to r^2 * exp(-r/a0) * cos^2(theta)
        // cos(theta) = z/r so r^2 simplifies:
        return z * z * std::exp(-r / a0);
    }   
};








// ==============================================================
// METROPOLIS ALGORITHM
// ==============================================================

// The Metropolis Algorithm Class
template <typename Distribution>
class MetropolisAlgorithm {
private:
    Distribution prob; // The distribution to sample from
    double _step = 0.5;
    string _tentative_distr = "uniform";
    int _accepted_moves = 0;

    // Tentative move function (Moved INSIDE as a private method)
    Position tentative_move(Position p, Random& rnd) {
        if (_tentative_distr == "uniform") {
            p.x += _step * (rnd.Rannyu() - 0.5) * 2.0; 
            p.y += _step * (rnd.Rannyu() - 0.5) * 2.0; 
            p.z += _step * (rnd.Rannyu() - 0.5) * 2.0; 
        } else if (_tentative_distr == "gaussian") {
            p.x = rnd.Gauss(p.x, _step); 
            p.y = rnd.Gauss(p.y, _step); 
            p.z = rnd.Gauss(p.z, _step); 
        } else {
            std::cerr << "Error: Unknown distribution type. Returning old position." << std::endl;
            return p;
        }
        return p;
    }

public:
    // Constructor initializes the probability distribution
    MetropolisAlgorithm(Distribution p, string tentative_distr_name = "uniform", double initial_step=0.5) : prob(p) {
        _tentative_distr = tentative_distr_name;
        _step = initial_step;
        _accepted_moves = 0;
    }

    double get_step() const {
        return _step;
    }

    void set_step(double step){
        _step = step;
    }

    string get_tentative_distr_type() const {
        return _tentative_distr;
    }

    string set_tentative_distr(string distr){
        _tentative_distr = distr;
    }

    int get_accepted_moves(){
        return _accepted_moves;
    }

    // Acceptance function
    double acceptance(Position p_old, Position p_new) {
        double p_n = prob(p_new);
        double p_o = prob(p_old);
        
        if (p_n >= p_o) return 1.0;
        else return p_n / p_o;
    }

    // Assume the tentative disrtibution is symmetric, i.e. T(x|y) = T(y|x): either uniform or gaussian
    void perform_metropolis_move(Position& current_p, Random& rnd) {
        
        // Propose a move according to the chosen distribution (now calling the private method)
        Position proposed_p = tentative_move(current_p, rnd);
        
        // Calculate acceptance probability
        double alpha = acceptance(current_p, proposed_p);
        
        // Extract a random number to decide whether to accept the move
        if (rnd.Rannyu() < alpha) { // Accept the move
            current_p = proposed_p; // Update position
            _accepted_moves++;       // Increment accepted moves counter
        }

        // If the move is rejected, current_p remains unchanged
    }

    // Tuning step
    void tune_step(Position& current_p, Random& rnd, double initial_step=0.5) {
        
        _step = initial_step;
        double target_acc = 0.50;
        double tolerance = 0.05; // 5% tolerance (window 45% - 55%)
        int n_steps_per_check = 10000; // Number of steps to have statistically significant acceptance measurement
        
        double current_acc = 0.0;
        
        std::cout << "\nStarting calibration for distribution:" << _tentative_distr << "\n" << std::endl;

        while (true) {

            _accepted_moves = 0; // Reset accepted moves counter for this tuning iteration
            
            // Run the metropolis for a certain number of steps and count how many moves are accepted
            for (int i = 0; i < n_steps_per_check; ++i) {
                perform_metropolis_move(current_p, rnd);
            }
            
            current_acc = static_cast<double>(_accepted_moves) / n_steps_per_check;
            
            std::cout << " =====   Tried step = " 
                      << std::fixed << std::setprecision(4) << std::left << std::setw(8) << _step 
                      << " ->   Acceptance = " 
                      << std::right << std::setw(6) << std::setprecision(2) << (current_acc * 100.0) 
                      << "%   ======" << std::endl;

            // If we are within the target acceptance window, we can stop tuning
            if (std::abs(current_acc - target_acc) <= tolerance) {
                std::cout << "\nCalibration completed! Step chosen = " << _step << "\n" << std::endl;
                break;
            } 
            // If accuracy is too high, make bigger steps
            else if (current_acc > target_acc) {
                _step *= 1.1; // Augment step by 10%
            } 
            // If accuracy is too low, make smaller steps
            else {
                _step *= 0.9; // Reduce step by 10%
            }
        }

    }

    // Equilibration

    void equilibrate(Position p, int n_equilibration, Random rnd){
        std::cout << "Starting equilibration phase ..." << std::endl;

        for(int i = 0; i < n_equilibration; i++) {
            perform_metropolis_move(p, rnd);
        }

        _accepted_moves = 0; // Reset accepted moves counter after equilibration
        std::cout << "Equilibration phase completed!" << std::endl << std::endl;
    }

};

#endif // METROPOLIS_H