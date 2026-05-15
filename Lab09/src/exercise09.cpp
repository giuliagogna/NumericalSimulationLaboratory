#include <iostream>
#include "GenerateCoords.h"
#include "Individual.h"
#include "Population.h"
#include <armadillo>

using namespace std;

int main() {

    // Global initialization
    int M = 34; // Number of cities in the TSP problem
    int pop_size = 700; // Size of the population for the genetic algorithm
    int n_generations = 700; // Number of generations to evolve the population
    string shape = "Square"; // Shape of the coordinate distribution (e.g., "Circ" for circumference, "Square" for square)

    // filename string: es "poplog_N34_Pop100_Gen200_Circ.dat"
    string filename = "poplog_N" + to_string(M) + 
                      "_Pop" + to_string(pop_size) + 
                      "_Gen" + to_string(n_generations) + 
                      "_" + shape + ".dat";

    if(M%2 != 0){
        cerr << "Error: The number of cities (M) must be even for the crossover operator to work properly." << endl;
        return EXIT_FAILURE;
    }
    if(pop_size%2 != 0){
        cerr << "Error: The population size (pop_size) must be even for the crossover operator to work properly." << endl;
        return EXIT_FAILURE;
    }

    Random rnd;
    InitializeGenerator(rnd); // Initialize the random number generator


    // ==============================================================================================
    // ================== COORDINATE GENERATION =====================================================
    // ==============================================================================================

    GenerateCoords gen_coords(M); // Example: generate coordinates for 34 cities on a circumference

    gen_coords.GenSquare(); // Generate coordinates on a square
    gen_coords.save_coords(); // Save the generated coordinates to a file

    mat coord_mat = gen_coords.get_coords_matrix(); // Get the coordinates matrix to pass to the Population constructor


    // ==============================================================================================
    // ================== GENETIC ALGORITHM =========================================================
    // ==============================================================================================

    Population population(pop_size, M, rnd, coord_mat);

    // Initialize the file to store the population history
    ofstream pop_log;
    OpenOutputFile(pop_log, filename);

    // Metadata header (Using '#' so Python knows to ignore these lines)
    pop_log << "# --- TSP Genetic Algorithm Run ---" << endl;
    pop_log << "# Number of Cities: " << M << endl;
    pop_log << "# Population Size: " << pop_size << endl;
    pop_log << "# Total Generations: " << n_generations << endl;
    pop_log << "# Map Shape: " << shape << endl;
    pop_log << "# ---------------------------------" << endl;

    // Header
    pop_log << "Generation Rank Fitness ";

    for(int j = 0; j < M + 1; j++) pop_log << "City_" << j+1 << " ";
    pop_log << endl;

    // Save generation 0 (initial random state)
    population.SavePopulationLog(pop_log, 0);

    // Start the loop at generation 1
    for(int i = 1; i <= n_generations; i++){

        population.EvolveOneGeneration(); 

        population.SavePopulationLog(pop_log, i);

        cout << "Generation " << i << " | Best Fitness: " << population.GetBestFitness() << endl;
    }

    // Don't forget to close it when the loop finishes!
    pop_log.close();

    

    return 0;
}