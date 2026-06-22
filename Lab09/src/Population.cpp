#include "Population.h"
#include "../../utils/auxiliary_functions/functions.h"
#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

// --- INITIALIZATION & SORTING ---

Population::Population(int pop_size, int n_cities, Random& rnd, const mat& coords) 
    : _pop_size(pop_size), _n_cities(n_cities), _coords(coords), _rnd(rnd) {

    if (_pop_size % 2 != 0) {
        cerr << "\nWARNING [Population]: The population size (" << _pop_size 
             << ") is odd. Crossover efficiency is slightly reduced because one child will be discarded per generation.\n" << endl;
    }
    if (_coords.n_rows != static_cast<unsigned int>(_n_cities)) {
        cerr << "Population Error: Mismatch between n_cities and coordinate matrix size." << endl;
        exit(1);
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
    idxs.push_back(1); // Ensure the tour ends at city 1

    // Fisher-Yates shuffle (locks index 0 as city 1)
    // Starting from _n_cities - 1 the last index is also fixed as city 1
    for(int i = _n_cities - 1; i > 0; i--){
        double r = _rnd.Rannyu(); 
        int j = 1 + static_cast<int>(r * i); 
        swap(idxs[i], idxs[j]); 
    }

    return idxs;
}

// Helper function to pass to sort algorithm
bool CompareFitness(const Individual& a, const Individual& b) {
    return a.get_fitness() < b.get_fitness();
}

void Population::SortByFitness() {
    sort(_pop.begin(), _pop.end(), CompareFitness);
}

// --- SELECTION ---

// Select an Individual from the Population
int Population::Select() {
    double r = _rnd.Rannyu();
    double p = 3.0; // The exponent. p > 1 privileges lower indices (better fitness)

    int j = static_cast<int>(_pop_size * pow(r, p));

    // To ensure the index is never out of bouds
    if(j >= _pop_size) { 
        cout << "In Select(): j out of bounds. j = " << j << endl;
        j = _pop_size - 1; 
    }
    
    return j;
}

// --- GENETIC OPERATORS ---

//  MUTATIONS

// Swaps the position of two cities in the tour
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

// Shift of +n positions for m contiguous cities (except for the first city and m < N-1)
void Population::MutateShift(vector<int>& child_idxs) {
    // Shifts cities from index start_index to start_index + n_to_shift - 1 by amount_to_shift positions to the right (circularly)
    int start_index = static_cast<int>(_rnd.Rannyu(1, _n_cities - 1)); // Starting index of the block to shift
    int n_to_shift = static_cast<int>(_rnd.Rannyu(1, _n_cities - start_index)); // Number of cities to shift

    int max_shift = _n_cities - (start_index + n_to_shift);
    if (max_shift < 1) return; // Safety check

    int amount_to_shift = static_cast<int>(_rnd.Rannyu(1, max_shift + 1)); // Amount to shift the cities

    vector<int> child_copy = child_idxs;

    for(int i = start_index; i < start_index + n_to_shift; i++){
        child_idxs[i + amount_to_shift] = child_copy[i];
    }

    for(int i = start_index; i < start_index + amount_to_shift; i++){
        child_idxs[i] = child_copy[i + n_to_shift];
    }

}

// Reverse the order of m contiguous cities
void Population::MutateInversion(vector<int>& child_idxs) {
    // Pick a starting point > 0
    int start_index = static_cast<int>(_rnd.Rannyu(1, _n_cities - 1));
    // Pick an ending point strictly greater than start
    int end_index = static_cast<int>(_rnd.Rannyu(start_index + 1, _n_cities));
    
    // Reverse the block of elements
    reverse(child_idxs.begin() + start_index, child_idxs.begin() + end_index + 1);
}

// Swaps two blocks of m < N_pop/2 contiguous cities
void Population::MutateBlockSwap(vector<int>& child_idxs) {

    int movable_cities = _n_cities - 1;
    int max_block_size = static_cast<int>(movable_cities / 2);
    int block_size = static_cast<int>(_rnd.Rannyu(1, max_block_size + 1));

    // start_block_1 is the left block
    // it must leave at least `block_size` spaces to its right for block 2 (so there has to be space for 2 blocks)
    int max_start_1 = movable_cities - (2 * block_size) + 1;
    int start_block_1 = static_cast<int>(_rnd.Rannyu(1, max_start_1 + 1));

    // start_block_2 is always on the right of block 1
    // it must start strictly after block 1 ends
    int min_start_2 = start_block_1 + block_size;
    // It must have renough room to finish
    int max_start_2 = movable_cities - block_size + 1;
    
    int start_block_2 = static_cast<int>(_rnd.Rannyu(min_start_2, max_start_2 + 1));

    for(int i = 0; i < block_size; i++) {
        swap(child_idxs[start_block_1 + i], child_idxs[start_block_2 + i]);
    }

}


//  CROSSOVER

void Population::Crossover(const vector<int>& parent1, const vector<int>& parent2, vector<int>& child1, vector<int>& child2) { // Pass child 1 and 2 as arguments: they will be hollow vectors that get filled with
                       // the indexes. This allows returning two vecs
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

    // Always save the best element of the previous generation
    new_population.push_back(_pop[0]);

    // Loop until the new population is completely full
    while (new_population.size() < _pop_size) {
        
        // Select two parents
        int p1_idx = Select();
        int p2_idx = Select();
        
        vector<int> parent1 = _pop[p1_idx].get_individual();
        vector<int> parent2 = _pop[p2_idx].get_individual();

        vector<int> child1, child2;
        
        // Apply Crossover
        double p_Crossover = 0.7; 
        if(_rnd.Rannyu() < p_Crossover) {
            Crossover(parent1, parent2, child1, child2);
        } else {
            child1 = parent1;
            child2 = parent2;
        }

        // Apply Mutations
        double p_Mutation = 0.1;
        if(_rnd.Rannyu() < p_Mutation) MutatePairPermutation(child1);
        if(_rnd.Rannyu() < p_Mutation) MutateShift(child1);
        if(_rnd.Rannyu() < p_Mutation) MutateInversion(child1);
        if(_rnd.Rannyu() < p_Mutation) MutateBlockSwap(child1);

        if(_rnd.Rannyu() < p_Mutation) MutatePairPermutation(child2);
        if(_rnd.Rannyu() < p_Mutation) MutateShift(child2);
        if(_rnd.Rannyu() < p_Mutation) MutateInversion(child2);
        if(_rnd.Rannyu() < p_Mutation) MutateBlockSwap(child2);

        // Add the first child
        new_population.push_back(Individual(child1, _coords));

        // Only add the second child if we haven't reached the limit yet
        if (new_population.size() < _pop_size) {
            new_population.push_back(Individual(child2, _coords));
        }
    }

    // Replace the old population with the new one
    _pop = new_population; 

    // Sort the new children
    SortByFitness(); 
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

void Population::SavePopulationLogSynth(ofstream& out_file, int generation) const {
    // _pop is already sorted from best (0) to worst (_pop_size - 1)
    for (int i = 0; i < _pop_size; i++) {
        // Write the metadata for all individuals
        out_file << generation << " "              
                 << i << " "                       
                 << _pop[i].get_fitness();

        // Write the full tour only for the absolute best individual (Rank 0)
        if (i == 0) {
            out_file << " ";
            vector<int> tour = _pop[i].get_individual();
            for (int city : tour) {
                out_file << city << " ";
            }
        }
        
        out_file << endl;
    }
}