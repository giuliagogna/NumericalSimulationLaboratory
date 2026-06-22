#include <iostream>
#include <cmath>
#include <fstream>
#include "../../utils/auxiliary_functions/functions.h"
#include "Individual.h"

using namespace std;
using namespace arma;


bool Individual::_is_valid() {
    // Check if the tour contains exactly _n_cities indices
    if (_idxs.size() != _n_cities + 1) {
        cerr << "Error: The tour must contain exactly " << _n_cities << " indices." << endl;
        return false;
    }

    // Check if the tour starts at city 1
    if(_idxs[0] != 1){
        cerr << "Error: The tour must start with city index 1." << endl;
        return false;
    }

    if(_idxs[_n_cities] != 1){
        cerr << "Error: The tour must end with city index 1." << endl;
        return false;
    }

    vector<bool> visited(_n_cities, false); // Vector to track visited cities

    for (int i = 0; i < _n_cities; i++) {
        if (_idxs[i] < 1 || _idxs[i] > _n_cities) {
            cerr << "Error: City index " << _idxs[i] << " is out of bounds. Valid indices are between 1 and " << _n_cities << "." << endl;
            return false;
        }
        if (visited[_idxs[i] - 1]) {
            // If the visited vector at position idx-1 is already true, it means tath said index is already passed into the for
            // loop, therefore the city is visited more than once
            cerr << "Error: City index " << _idxs[i] << " is visited more than once." << endl;
            return false;
        }
        visited[_idxs[i] - 1] = true; // Mark the city as visited
    }

    return true; // The tour is valid
}


double Individual::_compute_fitness(const mat& coords) {
    double tour_length = 0.0;

    for(int i=0; i < _n_cities; i++){
        int city_idx_1 = _idxs[i] - 1;    // Convert to 0-based index
        int city_idx_2 = _idxs[i+1] - 1;  // Convert to 0-based index

        double dx = coords(city_idx_1, 0) - coords(city_idx_2, 0);
        double dy = coords(city_idx_1, 1) - coords(city_idx_2, 1);

        tour_length += sqrt(dx * dx + dy * dy);
        
        // Last iteration of this loop closes the circle: city_idx_1 is the second-to-last city which is the last city before coming back to 1, city_idx_2 is the last city which is city 1
    }

    return tour_length;
}


Individual::Individual(const vector<int>& idxs, const mat& coords) { 
    _idxs = idxs;
    _n_cities = idxs.size() - 1; // The number of cities is the size of the tour minus one (since the tour starts and ends at city 1)
    _fitness = 0.0; 

    // Automatically validate the city sequence provided by the Population
    if(!_is_valid()){
        cerr << "Error: Invalid tour sequence provided to Individual constructor." << endl;
        cerr << "Generated sequence: ";
        for (int idx : _idxs) {
            cerr << idx << " ";
        }        
        cerr << endl;
        exit(EXIT_FAILURE);
    }

    // Calculate the fitness using the provided coordinates
    _fitness = _compute_fitness(coords);
}

double Individual::get_fitness() const { 
    return _fitness; 
}

vector<int> Individual::get_individual() const { 
    return _idxs; 
}

int Individual::get_n_cities() const { 
    return _n_cities; 
}

void Individual::save_tour(const string& filename) {
    ofstream file;
    OpenOutputFile(file, filename);

    for (int idx : _idxs) {
        file << idx << endl;
    }

    file.close();
}