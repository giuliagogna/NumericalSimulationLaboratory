#include "Population.h"
#include <iostream>
#include <cmath>

using namespace std;

// --- INITIALIZATION & SORTING ---

Population::Population(int pop_size, int n_cities, Random& rnd, const mat& coords) 
    : _pop_size(pop_size), _n_cities(n_cities), _coords(coords), _rnd(rnd) {
    
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
        cout << "Unexpected generator result! INDEX IN POPULATION BIGGER THAT NUNBER OF ELEMENTS IN POPULATION" << endl;
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

// Before adding the crossover, check if the genetic operators perform well

// ========================================
// EVOLUTIONARY ENGINE
// ========================================

void Population::EvolveOneGeneration(){

    SortByFitness(); // Sort the population by fitness before applying selection and mutation

    vector<Individual> new_population;

    // Select an individual
    for(int i = 0; i < _pop_size; i++){
        //cout << "Evolving individual " << i+1 << "/" << _pop_size << endl; // Progress indicator
        

        int individual_index = Select();
        vector<int> selected_individual = _pop[individual_index].get_individual();

        double p_Mutation = 0.07;

        if(_rnd.Rannyu() < p_Mutation) MutatePairPermutation(selected_individual);
        if(_rnd.Rannyu() < p_Mutation) MutateShift(selected_individual);
        if(_rnd.Rannyu() < p_Mutation) MutateInversion(selected_individual);
        if(_rnd.Rannyu() < p_Mutation) MutateBlockSwap(selected_individual);

        new_population.push_back(Individual(selected_individual, _coords));

    }

    _pop = new_population; // Replace the old population with the new one

}