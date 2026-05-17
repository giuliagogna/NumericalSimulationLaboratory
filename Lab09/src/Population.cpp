#include "Population.h"
#include "../../utils/auxiliary_functions/functions.h"
#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

// --- INITIALIZATION & SORTING ---

Population::Population(int pop_size, int n_cities, Random& rnd, const mat& coords) 
    : _pop_size(pop_size), _n_cities(n_cities), _coords(coords), _rnd(rnd) {

    if (_n_cities % 2 != 0) {
        throw std::invalid_argument("Population Error: The number of cities (M) must be even for crossover.");
    }
    if (_pop_size % 2 != 0) {
        throw std::invalid_argument("Population Error: The population size must be even.");
    }
    if (_coords.n_rows != static_cast<unsigned int>(_n_cities)) {
         throw std::invalid_argument("Population Error: Mismatch between n_cities and coordinate matrix size.");
    }
    
    for(int i = 0; i < _pop_size; i++) {
        vector<int> random_tour = GenerateRandomTour();
        _pop.push_back(Individual(random_tour, _coords));
    }
    SortByFitness();
}

vector<int> Population::GenerateRandomTour() {
    vector<int> idxs;
    for(int i = 0; i < _n_cities; i++){
        idxs.push_back(i+1); 
    }
    idxs.push_back(1); // Ensure the tour ends at city 1 (index 0 in 0-based)

    // Fisher-Yates shuffle (locks index 0 as city 1)
    // Starting from _n_cities - 1 the last index is also fixed as city 1
    for(int i = _n_cities - 1; i > 0; i--){
        double r = _rnd.Rannyu(); 
        int j = 1 + static_cast<int>(r * i); 
        swap(idxs[i], idxs[j]); 
    }

    return idxs;
}

bool CompareFitness(const Individual& a, const Individual& b) {
    // Helper function to pass to sort algorithm
    return a.get_fitness() < b.get_fitness();
}

void Population::SortByFitness() {
    sort(_pop.begin(), _pop.end(), CompareFitness);
}

// --- SELECTION ---

// Select an Individual from the Population
int Population::Select() {
    double r = _rnd.Rannyu();
    double p = 2.0; // The exponent. p > 1 privileges lower indices (better fitness)

    // Relying on the fact that the generator never generates exactly 1 (otherwise quite a bit of a problem)
    int j = static_cast<int>(_pop_size * pow(r, p)) + 1;

    // To ensure the index is never out of bouds
    if(j >= _pop_size) { 
        j = _pop_size - 1; 
    }
    
    return j;
}

// --- GENETIC OPERATORS ---

//  MUTATIONS
void Population::MutatePairPermutation(vector<int>& child_idxs) {
    // Pick two random positions, strictly greater than 0 (never move city 1)
    // Relying on the fact that generator never extract exactly 1 and so the maximum integer part that can result
    // is _n_cities - 1 that ensures that the last index of the Individual is not touched
    int idx1 = static_cast<int>(_rnd.Rannyu(1, _n_cities));
    int idx2 = static_cast<int>(_rnd.Rannyu(1, _n_cities));

    // Ensure we don't pick the exact same index twice: preserve correct probability of mutation
    while (idx1 == idx2) {
        idx2 = static_cast<int>(_rnd.Rannyu(1, _n_cities));
    }

    swap(child_idxs[idx1], child_idxs[idx2]);
}

void Population::MutateShift(vector<int>& child_idxs) {
    // Shifts cities from index start_index to start_index + n_to_shift - 1 by shift_amount positions to the right (circularly)
    int start_index = static_cast<int>(_rnd.Rannyu(1, _n_cities - 1));           // Starting index of the block to shift
    int n_to_shift = static_cast<int>(_rnd.Rannyu(1, _n_cities - start_index));   // Number of cities to shift

    int max_shift = _n_cities - (start_index + n_to_shift);
    if (max_shift < 1) return; // Safety check

    int amount_to_shift = static_cast<int>(_rnd.Rannyu(1, max_shift + 1)); // Amount to shift the cities

    vector<int> child_copy = child_idxs;

    for(int i = start_index; i < start_index + n_to_shift; i++){
        child_idxs[i + amount_to_shift] = child_copy[i];
    }

    for(int i = start_index + n_to_shift; i < start_index + n_to_shift + amount_to_shift; i++){
        child_idxs[i - n_to_shift] = child_copy[i];
    }

}

void Population::MutateInversion(vector<int>& child_idxs) {
    // Pick a starting point > 0
    int start_index = static_cast<int>(_rnd.Rannyu(1, _n_cities - 1));
    // Pick an ending point strictly greater than start
    int end_index = static_cast<int>(_rnd.Rannyu(start_index + 1, _n_cities));
    
    // Reverse the block of elements
    reverse(child_idxs.begin() + start_index, child_idxs.begin() + end_index + 1);
}

void Population::MutateBlockSwap(vector<int>& child_idxs) {

    int movable_cities = _n_cities - 1;
    int max_block_size = static_cast<int>(movable_cities / 2);
    int block_size = static_cast<int>(_rnd.Rannyu(1, max_block_size + 1));

    // start_block_1 is the left block
    // it must leave at least `block_size` spaces to its right for block 2
    int max_start_1 = movable_cities - (2 * block_size) + 1;
    int start_block_1 = static_cast<int>(_rnd.Rannyu(1, max_start_1 + 1));

    // start_block_2 is always on the right of block 1
    // it must start strictly after block 1 ends, and have room to finish
    int min_start_2 = start_block_1 + block_size;
    int max_start_2 = movable_cities - block_size + 1;
    int start_block_2 = static_cast<int>(_rnd.Rannyu(min_start_2, max_start_2 + 1));

    for(int i = 0; i < block_size; i++) {
        swap(child_idxs[start_block_1 + i], child_idxs[start_block_2 + i]);
    }

}


//  CROSSOVER

void Population::Crossover(const vector<int>& parent1, const vector<int>& parent2, vector<int>& child1, vector<int>& child2) {
    int size = parent1.size();

    // random cut position: avoid index 0 (city 1) and the last index (also city 1)
    int cut = static_cast<int>(_rnd.Rannyu(1, size - 1));

    // conserve the first part of the paths by copying the parents into the children
    child1 = parent1;
    child2 = parent2;

    // complete the paths with missing cities in the other parent's order
    int insert_idx1 = cut;
    int insert_idx2 = cut;

    // iterate through the parents from left to right (skipping index 0)
    for (int i = 1; i < size; i++) {
        
        // process child 1 using parent 2 order
        int candidate1 = parent2[i];
        bool found1 = false;
        
        // check if candidate1 is already in the conserved part of child1
        for (int j = 1; j < cut; j++) {
            if (child1[j] == candidate1) {
                found1 = true;
                break;
            }
        }
        
        // if it was not in the conserved part, it's a missing city, so add it
        if (!found1 && insert_idx1 < size) {
            child1[insert_idx1] = candidate1;
            insert_idx1++;
        }


        // process child 2 using parent 1 order
        int candidate2 = parent1[i];
        bool found2 = false;
        
        // Check if candidate2 is already in the conserved part of child2
        for (int j = 1; j < cut; j++) {
            if (child2[j] == candidate2) {
                found2 = true;
                break;
            }
        }
        
        // if it was not in the conserved part, it's a missing city, so add it
        if (!found2 && insert_idx2 < size) {
            child2[insert_idx2] = candidate2;
            insert_idx2++;
        }
    }
}

// ========================================
// EVOLUTIONARY ENGINE
// ========================================

void Population::EvolveOneGeneration(){

    vector<Individual> new_population;

    // Increment by two individual per cycle: crossover takes in two parents and produces two children, so at each iteration 
    // I make two processings. At the end of the cycle _pop_size processings have been done
    for(int i = 0; i < _pop_size; i += 2){
        
        // Select two parents
        int p1_idx = Select();
        int p2_idx = Select();
        
        vector<int> parent1 = _pop[p1_idx].get_individual();
        vector<int> parent2 = _pop[p2_idx].get_individual();

        // Vectors that will host the children: they will be filled by the crossover function, but they need to be defined here to be passed as arguments
        vector<int> child1, child2;
        
        // Probability of crossover
        double p_Crossover = 0.65; 

        // Apply Crossover
        if(_rnd.Rannyu() < p_Crossover) {
            Crossover(parent1, parent2, child1, child2);
        } else {
            // If crossover does not trigger, the children are exact clones of the parents
            child1 = parent1;
            child2 = parent2;
        }

        // Apply mutations independently for the two children
        double p_Mutation = 0.07;

        if(_rnd.Rannyu() < p_Mutation) MutatePairPermutation(child1);
        if(_rnd.Rannyu() < p_Mutation) MutateShift(child1);
        if(_rnd.Rannyu() < p_Mutation) MutateInversion(child1);
        if(_rnd.Rannyu() < p_Mutation) MutateBlockSwap(child1);

        if(_rnd.Rannyu() < p_Mutation) MutatePairPermutation(child2);
        if(_rnd.Rannyu() < p_Mutation) MutateShift(child2);
        if(_rnd.Rannyu() < p_Mutation) MutateInversion(child2);
        if(_rnd.Rannyu() < p_Mutation) MutateBlockSwap(child2);

        // The children get added to the new population: add both
        // I added a safety check in the main: if the population size provided is odd, the program gives error
        new_population.push_back(Individual(child1, _coords));
        new_population.push_back(Individual(child2, _coords));
    }

    _pop = new_population; // Replace the old population with the new one

    SortByFitness(); // Instantly sort the new children!

}

void Population::SavePopulationLog(ofstream& out_file, int generation) const {
    // _pop is already sorted from best (0) to worst (_pop_size - 1)
    for (int i = 0; i < _pop_size; i++) {
        // Write the metadata
        out_file << generation << " "              
                 << i << " "                       
                 << _pop[i].get_fitness() << " ";  

        // Write the tour
        vector<int> tour = _pop[i].get_individual();
        for (int city : tour) {
            out_file << city << " ";
        }
        
        out_file << endl;
    }
}