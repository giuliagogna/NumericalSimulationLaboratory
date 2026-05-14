// This file contains the functions that generate the coordinates of the cities in the TSP problem
// These functions will be called only once in the main function when constructing the problem (cities coordinates are fixed throughout the algorithm)
// This file will contain a method used to store the coordinates of the cities in a matrix _n_cities x 2
// The coordinates are in the cartesian frame of reference

#ifndef GENERATE_COORDS_H
#define GENERATE_COORDS_H

#include <armadillo>
#include <cstdlib>
#include <string>
#include "../../utils/random/random.h"
#include "../../utils/auxiliary_functions/functions.h"

using namespace std;
using namespace arma;

class GenerateCoords {

private:

    Random _rnd; // Random number generator for generating random coordinates of cities
    mat _coords; // Matrix to store the coordinates of the cities (e.g., x and y coordinates for 2D TSP)
    int _n_cities; // Number of cities to generate coordinates for
    string _coord_type; // Type of coordinate generation (e.g., "circumference" or "square")

public:

    // The constructor will take as input the number of cities and the type of coordinate and will provide the coordinates of the cities in the _coords matrix

    GenerateCoords(int n_cities) { // Constructor to initialize the number of cities
        _n_cities = n_cities;
        _coords.resize(_n_cities, 2); // 2D coordinates (x and y)

        // Initialize the random number generator
        InitializeGenerator(_rnd);
    }

    void GenCirc(){ // Generate coordinates of cities uniformly distributed on a circumference of unit radius: fills the coords matrix
        _coord_type = "circumference"; // Set the coordinate type for file naming
        double theta;
        for(int i=0; i<_n_cities; i++){
            theta = _rnd.Rannyu(0, 2*M_PI); // Generate a random angle between 0 and 2*pi
            _coords(i, 0) = cos(theta); // x coordinate
            _coords(i, 1) = sin(theta); // y coordinate
        }
    }; 

    void GenSquare(){ // Generate coordinates of cities uniformly distributed inside a square of unit side: fills the coords matrix
        _coord_type = "square"; // Set the coordinate type for file naming
        for(int i=0; i<_n_cities; i++){
            _coords(i, 0) = _rnd.Rannyu(0, 1); // x coordinate
            _coords(i, 1) = _rnd.Rannyu(0, 1); // y coordinate
        }
    }; 

    
    pair<double, double> get_coords(int city_index) { // Method to get the coordinates of a specific city by its index
        if (city_index < 0 || city_index >= _n_cities) {
            cerr << "Error: City index out of bounds: " << city_index << endl;
            exit(EXIT_FAILURE);
        }
        return make_pair(_coords(city_index, 0), _coords(city_index, 1)); // Return x and y coordinates as a pair
    };

    mat get_coords_matrix() const { // Method to get the entire coordinates matrix
        return _coords;
    }

    void save_coords(){ // Method to save the coordinates of the cities to a file in the OUTPUT directory
        ofstream coutf;
        OpenOutputFile(coutf, string("Coords_") + _coord_type + ".dat");

        coutf << "# City_Index x_coordinate y_coordinate" << endl;
        for(int i=0; i<_n_cities; i++){
            // Let the city coordinates in the file go from 1 to _n_cities
            coutf << setw(12) << i + 1 
                  << setw(12) << _coords(i, 0)
                  << setw(12) << _coords(i, 1) << endl;
        }
        coutf.close();

    }

};

#endif // GENERATE_COORDS_H