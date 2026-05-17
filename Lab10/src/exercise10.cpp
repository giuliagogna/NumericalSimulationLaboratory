#include <iostream>
#include <string>
#include <armadillo>

#include "Individual.h"
#include "Population.h"
#include "Initializer.h"

#include "../../utils/auxiliary_functions/functions.h"

#include "mpi.h"

using namespace std;
using namespace arma;

int main(int argc, char* argv[]) {

    // =========================================================
    // MPI INITIALIZATION
    // =========================================================
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;

    try {
        // CONFIGURATION
        Config config = ReadInput("input/input.dat");
        Random rnd;
        InitializeMPI_Generator(rnd, rank); // Initialize the random generator with different seeds for each process

        // COORDINATE SETUP
        mat coord_mat = SetupCoordinates(config, rank); // Only rank 0 generates the coordinates and writes to screen
        int M = config.N_CITIES; // Number of cities

        if(rank == 0) cout << "Initializing " << size << " Continents..." << endl;

        // POPULATION SETUP
        Population population(config.POP_SIZE, M, rnd, coord_mat);

        // LOGGING SETUP
        ofstream pop_log;
        ofstream output_log;

        string filename = SetupLoggers(pop_log, output_log, config, rank, size);

        // Save Generation 0 (only rank 0 writes)
        if (rank == 0) {
            population.SavePopulationLog(pop_log, 0);
            output_log << "Generation 0 | Best Fitness: " << population.GetBestFitness() << endl;
        }

        // EVOLUTIONARY ENGINE
        for(int i = 1; i <= config.N_GENERATIONS; i++){
            population.EvolveOneGeneration(); 

            // MIGRATION
            if(i % config.N_MIGR == 0){

                int chosen_nodes[2] = {-1, -1};

                if (rank==0){ // Only rank 0 extracts
                    chosen_nodes[0] = static_cast<int>(rnd.Rannyu(0, size - 1));
                    chosen_nodes[1] = static_cast<int>(rnd.Rannyu(0, size - 1));

                    while(chosen_nodes[0] == chosen_nodes[1]){
                        chosen_nodes[1] = static_cast<int>(rnd.Rannyu(0, size - 1));
                    }
                }

                // Broadcast the two chosen nodes to everyone
                MPI_Bcast(chosen_nodes, 2, MPI_INTEGER, 0, MPI_COMM_WORLD);

                // Let the two selected nodes know their partners
                int partner = -1; // partner is -1 for everyone but the two chosen nodes

                if(rank==chosen_nodes[0]){
                    partner = chosen_nodes[1];
                } else if(rank==chosen_nodes[1]){
                    partner = chosen_nodes[0];
                }

                if(partner!=-1){
                    vector<int> best_tour = population.GetBestTour();
                    vector<int> recive_best(best_tour.size()); // Empty buffer to recive the foreigner

                    MPI_Sendrecv(best_tour.data(), best_tour.size(), MPI_INTEGER, partner, 0, recive_best.data(), recive_best.size(), MPI_INTEGER, partner, 0, MPI_COMM_WORLD, &stat);

                    population.ExchangeBest(recive_best);
                }

            }

            // Gather the information from all the node: only save the best individual
            double my_best_fitness = population.GetBestFitness();
            double global_best_fitness = 0.0;

            // Reduce: compare all 7 fitnesses, find the minimum, and send it to Rank 0
            MPI_Reduce(&my_best_fitness, &global_best_fitness, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

            if (rank == 0) {
                population.SavePopulationLog(pop_log, i);
                
                if (i % 10 == 0 || i == config.N_GENERATIONS) {
                    output_log << "Generation " << i << " | Best Fitness: " << population.GetBestFitness() << endl;
                }
            }
        
        }

        // CLEANUP (only rank 0 writes)

        if (rank == 0) {
            output_log << "-----------------------------------------\n";
            output_log << "Evolution complete! Master log saved to: " << filename << endl;
            pop_log.close();
            output_log.close();
            cout << "Simulation successful! Check outputs/output.dat for the summary report." << endl;
        }

    } 
    catch (const exception& e) {
        cerr << "\nCRITICAL " << e.what() << endl;
        return EXIT_FAILURE;
    }

    MPI_Finalize();

    return 0;
}