#include <iostream>
#include "GenerateCoords.h"
#include "Individual.h"
#include "Population.h"
#include <armadillo>

using namespace std;

int main() {

    // Global initialization
    int M = 10; // Number of cities in the TSP problem
    int pop_size = 100; // Size of the population for the genetic algorithm
    int n_generations = 100;

    Random rnd;
    InitializeGenerator(rnd); // Initialize the random number generator

    GenerateCoords gen_coords(M); // Example: generate coordinates for 34 cities on a circumference

    gen_coords.GenCirc(); // Generate coordinates on a circumference
    gen_coords.save_coords(); // Save the generated coordinates to a file

    mat coord_mat = gen_coords.get_coords_matrix(); // Get the coordinates matrix to pass to the Population constructor

    Population population(pop_size, M, rnd, coord_mat); // Create a population with the specified size and number of cities (coordinates will be set later)
    const string filename = "Initial_Best_Tour.dat";
    population.SaveBestTour(filename); // Save the best tour of the initial population to a file

    population.EvolveOneGeneration(); // Evolve the population for one generation
    population.SaveBestTour("Best_Tour_After_One_Generation.dat"); // Save the best tour after one generation to a file

    for(int i = 0; i<n_generations; i++){
        cout << "Generation: " << i << endl;
        population.EvolveOneGeneration(); // Evolve the population for one generation
    }

    population.SaveBestTour("Best_Tour_After_" + to_string(n_generations) + "_Generations.dat");

    

    return 0;
}