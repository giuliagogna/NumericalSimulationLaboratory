#include <iostream>
#include <string>
#include <armadillo>

#include "Individual.h"
#include "Population.h"
#include "Initializer.h"

using namespace std;
using namespace arma;

int main() {

    // CONFIGURATION
    Config config = ReadInput("input/input.dat");
    Random rnd;
    InitializeGenerator(rnd);

    // COORDINATE SETUP
    mat coord_mat = SetupCoordinates(config);

    // POPULATION INITIALIZATION
    Population population(config.POP_SIZE, config.N_CITIES, rnd, coord_mat);

    // LOGGING SETUP
    ofstream pop_log;
    ofstream output_log;
        
    string filename = SetupLoggers(pop_log, output_log, config); 

    // Save Generation 0
    population.SavePopulationLog(pop_log, 0);
    output_log << "Generation 0 | Best Fitness: " << population.GetBestFitness() << endl;

    // EVOLUTIONARY ENGINE
    for(int i = 1; i <= config.N_GENERATIONS; i++){
        population.EvolveOneGeneration(); 
        population.SavePopulationLog(pop_log, i);
            
        if (i % 10 == 0 || i == config.N_GENERATIONS) {
            output_log << "Generation " << i << " | Best Fitness: " << population.GetBestFitness() << endl;
        }
    }

    // CLEANUP
    output_log << "-----------------------------------------\n";
    output_log << "Evolution complete! Master log saved to: " << filename << endl;
        
    pop_log.close();
    output_log.close();

    cout << "Simulation successful! Check outputs/output.dat for the summary report." << endl;

    return 0;
}