#ifndef METROPOLIS_H
#define METROPOLIS_H

#include <iostream>
#include <cmath>
#include <string>

#include "../random/random.h"
#include "../auxiliary_functions/functions.h"

using namespace std;

// Position in 3D space
struct Position {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

// ==============================================================
// DISTRIBUTIONS TO SAMPLE
// ==============================================================

// Note: avoid all normalization constants, since they cancel in the acceptance ratio

// Hydrogen ground state wavefunction (1s orbital) (squared module)
struct Psi100{
    double a0 = 1.0; // Bohr radius in atomic units
    double operator()(Position p) const {
        double r = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);

        // Return |Psi|^2 proportional to exp(-2r/a0)
        return exp(-2.0 * r / a0);
    }
};

// Hydrogen 2p orbital wavefunction (n=2, l=1, m=0) (squared module)
struct Psi210{
    double a0 = 1.0; // Bohr radius in atomic units
    double operator()(Position p) const {
        double r = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
        double z = p.z;

        // |Psi|^2 is proportional to r^2 * exp(-r/a0) * cos^2(theta)
        // cos(theta) = z/r so r^2 simplifies:
        return z * z * exp(-r / a0);
    }   
};


// EXERCISE 8)
// The probability density to sample is the squared modulus of the trial wavefunction
// Ψ ∝ e^[-(x-μ)/2σ^2] + e^[-(x+μ)/2σ^2]
// All normalization factors and constants drop since this function is used in the acceptance
// rate

struct Psi_T{
    double mu = 0.0;
    double sigma = 1.0;

    double operator()(double x) const {
        double arg1 = - (x * x) / (sigma*sigma); // argument of the exponentiaò
        double arg2 = (x * mu) / (sigma*sigma); // argument of the cosh
        return exp(arg1) * pow(cosh(arg2), 2);
    }   
};







// ==============================================================
// METROPOLIS ALGORITHM
// ==============================================================

// The Metropolis Algorithm Class
template <typename State, typename Distribution>
class MetropolisAlgorithm {
private:
    Distribution prob; // The distribution to sample from
    double _step = 0.5;
    string _tentative_distr = "uniform";
    int _accepted_moves = 0;

    // Using generical State allows to use a 3D or a 1D double: the type State is
    // evaluated runtime thanks to constexpr
    State _initial_position;

    // Tentative move function
    State tentative_move(State p, Random& rnd) {

        if (_tentative_distr == "uniform") {
            // Compile-time check: is the State a Position? (3D Metropolis)
            if constexpr (is_same_v<State, Position>){
                p.x += _step * (rnd.Rannyu() - 0.5) * 2.0; 
                p.y += _step * (rnd.Rannyu() - 0.5) * 2.0; 
                p.z += _step * (rnd.Rannyu() - 0.5) * 2.0; 
            }
            // Compile-time check: is the State a 1D double? (1D Metropolis)
            else if constexpr (std::is_same_v<State, double>) {
                p += _step * (rnd.Rannyu() - 0.5) * 2.0;
            }

        } else if (_tentative_distr == "gaussian") {
            if constexpr (is_same_v<State, Position>){
                p.x = rnd.Gauss(p.x, _step); 
                p.y = rnd.Gauss(p.y, _step); 
                p.z = rnd.Gauss(p.z, _step); 
            }
            else if constexpr(is_same_v<State, double>){
                p = rnd.Gauss(p, _step);
            }
        
        } else {
            cerr << "Error: Unknown distribution type. Returning old position." << endl;
            return p;
        }
        return p;
    }

public:
    // Constructor initializes the probability distribution
    MetropolisAlgorithm(Distribution p, State initial_position, string tentative_distr_name = "uniform", double initial_step=0.5) : prob(p) {
        _tentative_distr = tentative_distr_name;
        _step = initial_step;
        _accepted_moves = 0;
        _initial_position = initial_position;
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

    void reset_position(Position& p) const {
        p = _initial_position;
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
        
        cout << "\nStarting calibration for distribution: " << _tentative_distr << "\n" << endl;

        while (true) {

            _accepted_moves = 0; // Reset accepted moves counter for this tuning iteration
            
            // Run the metropolis for a certain number of steps and count how many moves are accepted
            for (int i = 0; i < n_steps_per_check; ++i) {
                perform_metropolis_move(current_p, rnd);
            }
            
            current_acc = static_cast<double>(_accepted_moves) / n_steps_per_check;
            
            cout << " =====   Tried step = " 
                      << fixed << setprecision(4) << left << setw(8) << _step 
                      << " ->   Acceptance = " 
                      << right << setw(6) << setprecision(2) << (current_acc * 100.0) 
                      << "%   ======" << endl;

            // If we are within the target acceptance window, we can stop tuning
            if (abs(current_acc - target_acc) <= tolerance) {
                cout << "\nCalibration completed! Step chosen = " << _step << "\n" << endl;
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

    void equilibrate(Position& p, int& n_equilibration, Random& rnd){
        cout << "Starting equilibration phase ..." << endl;

        for(int i = 0; i < n_equilibration; i++) {
            perform_metropolis_move(p, rnd);
        }

        _accepted_moves = 0; // Reset accepted moves counter after equilibration
        cout << "Equilibration phase completed!" << endl << endl;
    }

};

#endif // METROPOLIS_H