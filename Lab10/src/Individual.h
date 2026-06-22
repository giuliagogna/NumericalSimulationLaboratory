// This file contains the properties of the Individual. In the TSP problem, an individual is a sequence of cities (therefore a sequence of indexes) with associated fitness.
// The generation of the initial tour is moved to the Population class

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <armadillo>
#include "../../utils/random/random.h"
#include <vector>

using namespace std;
using namespace arma;

class Individual {

    // Population can access the private memebers in Indvidual
    friend class Population;

private:

    int _n_cities;     // Number of cities in the TSP problem
    vector<int> _idxs; // Vector of indices representing the order of cities in the TSP tour
    double _fitness;   // Fitness of the individual (e.g., total distance of the tour

    bool _is_valid();         // Function to check if the individual's tour is valid 
                              // (e.g., contains all cities exactly once and always starts and ends in 1)
    double _compute_fitness(const mat& coords);   // Method to compute the fitness of the individual 
                                                // (e.g., total distance of the tour based on the coordinates of the cities)

     // Constructor: initializes the _idxs vector, the number of cities with the length of the vector and calculates fitness using the provided coordinates
     // Since the constructor is private the main is prohibited to create an Individual: only Population can access this now
     Individual(const vector<int>& idxs, const mat& coords);

public:
    
    // Getters
    double get_fitness() const;
    vector<int> get_individual() const; 
    int get_n_cities() const;

    // Save the tour to a file (for data analysis)
    void save_tour(const string& filename);

};

#endif // INDIVIDUAL_H