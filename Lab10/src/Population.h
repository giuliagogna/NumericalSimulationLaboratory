#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <algorithm>
#include "Individual.h"
#include "../../utils/random/random.h"

using namespace std;
using namespace arma;

class Population {
private:
    int _pop_size;
    int _n_cities;
    vector<Individual> _pop;
    mat _coords; 
    Random& _rnd; 

    // Internal helper methods
    void SortByFitness(); 
    int Select(); 
    vector<int> GenerateRandomTour(); 

    // Genetic Operators
    void MutatePairPermutation(vector<int>& child_idxs);
    void MutateShift(vector<int>& child_idxs);
    void MutateInversion(vector<int>& child_idxs);
    void MutateBlockSwap(vector<int>& child_idxs);
    
    void Crossover(const vector<int>& parent1, const vector<int>& parent2, vector<int>& child1, vector<int>& child2);

public:
    // Constructor
    Population(int pop_size, int n_cities, Random& rnd, const mat& coords);

    // Main Engine
    void EvolveOneGeneration();

    // Getters for data analysis
    double GetBestFitness() const { return _pop[0].get_fitness(); }
    vector<int> GetBestTour() const { return _pop[0].get_individual(); }

    // Codify the random excange of the best individual between two random populations
    void ExchangeBest(const std::vector<int>& foreign_tour);

    // Save the best tour to a file (for data analysis)
    void SaveBestTour(const string& filename) {
        _pop[0].save_tour(filename);
    }

    // Save the entire population's fitness and individuals
    void SavePopulationLog(ofstream& out_file, int generation) const;
};

#endif // POPULATION_H