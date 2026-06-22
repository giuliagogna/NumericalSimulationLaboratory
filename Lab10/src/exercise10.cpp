#include <iostream>
#include <string>
#include <armadillo>
#include <filesystem>

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

    // Configuration
    Config config = ReadInput("input/input.dat");
    Random rnd;

    // Initialize the random generator with different seeds for each process
    InitializeMPI_Generator(rnd, rank);

    // Coorfinate setup
    mat coord_mat = SetupCoordinates(config, rank); // Only rank 0 generates the coordinates and writes to file
    int M = config.N_CITIES; // Number of cities

    if(rank == 0) cout << "Initializing " << size << " Continents..." << endl;

    // Population setup
    Population population(config.POP_SIZE, M, rnd, coord_mat);

    // Logging setup
    ofstream pop_log; // Kept to not break SetupLoggers function, but will be ignored
    ofstream output_log;

    string filename = SetupLoggers(pop_log, output_log, config, rank, size);

    // Track fitness of generation 0
    double my_gen0_best_fitness = population.GetBestFitness();
    double global_gen0_absolute_best_fitness = 0.0;
    // Communicate the best of all continents' fitness to rank 0
    MPI_Reduce(&my_gen0_best_fitness, &global_gen0_absolute_best_fitness, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    // Only Rank 0 writes to the global summary output_log
    if (rank == 0) {
        output_log << "Generation 0 | Best Fitness: " << global_gen0_absolute_best_fitness << endl;
    }
    

    // Evolutionary engine
    for(int i = 1; i <= config.N_GENERATIONS; i++){
        population.EvolveOneGeneration(); 

        // Migration
        if(i % config.N_MIGR == 0){

            int chosen_nodes[2] = {-1, -1};

            if (rank==0){ // Only rank 0 extracts
                chosen_nodes[0] = static_cast<int>(rnd.Rannyu(0, size));
                chosen_nodes[1] = static_cast<int>(rnd.Rannyu(0, size));

                while(chosen_nodes[0] == chosen_nodes[1]){
                    chosen_nodes[1] = static_cast<int>(rnd.Rannyu(0, size));
                }
            }

            // Broadcast the two chosen nodes to everyone
            MPI_Bcast(chosen_nodes, 2, MPI_INT, 0, MPI_COMM_WORLD);

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

                MPI_Sendrecv(best_tour.data(), best_tour.size(), MPI_INT, partner, 0, recive_best.data(), recive_best.size(), MPI_INT, partner, 0, MPI_COMM_WORLD, &stat);

                population.ExchangeBest(recive_best);
            }

        }

            // Gather the information from all the node: only save the best individual
            double my_best_fitness = population.GetBestFitness();
            double global_best_fitness = 0.0;

            // Reduce: compare all 7 fitnesses, find the minimum, and send it to Rank 0
            MPI_Reduce(&my_best_fitness, &global_best_fitness, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

            if (rank == 0){
                // Write the global best fitness across all continents for every generation
                output_log << "Generation " << i << " | Best Fitness: " << global_best_fitness << endl;
            }
             
        }

    pop_log.close(); // Close unused pop_log

    // Save only the absolute best tour among all continents

    double local_fitness = population.GetBestFitness();
    double global_fitness = 0.0;

    // Same as Reduce + Bcast: every continent finds its own best fitness and the function
    // finds the minimum and communicates it to the whole world
    MPI_Allreduce(&local_fitness, &global_fitness, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

    // Initialize with a numebr grater than any other rank
    int my_candidate_rank = size + 1;

    if (local_fitness == global_fitness){
        my_candidate_rank = rank;
    }

    int global_champ_rank = -1.0;
    // If two nodes found the exact same fitness, the first to nominate itself wins
    MPI_Allreduce(&my_candidate_rank, &global_champ_rank, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    // Extract the absolute best tour
    if (rank == 0){
        // Buffer to receive the best tour only exists in rank 0
        vector<int> global_best_tour;

        if (global_champ_rank == 0){ // 0 already has the winning tour
            global_best_tour = population.GetBestTour();
        } else { // 0 receives it from the winning rank
            global_best_tour.resize(population.GetBestTour().size());

            MPI_Recv(global_best_tour.data(), global_best_tour.size(), MPI_INT, global_champ_rank, 1, MPI_COMM_WORLD, &stat);
        }

        // Write the best tour in a file
        ofstream best_tour;
        OpenOutputFile(best_tour, "best_tour.dat");

        best_tour << "# Fitness ";

        for (int i = 1; i < population.GetBestTour().size() + 1; i++) {
            best_tour << "City" << i << " ";
        }

        best_tour << endl;

        best_tour << global_fitness << " ";

        for (int city : global_best_tour) {
            best_tour << city << " ";
        }

        best_tour << endl;
        best_tour.close();

        output_log << "-----------------------------------------\n";
        output_log << "Evolution complete! Master log saved to: " << filename << endl;

        output_log << "\nSimulation successful! Global champion found on Continent " << global_champ_rank << endl;
        output_log << "Check outputs/best_tour.dat for the final map sequence." << endl;
        output_log.close();

        cout << "Simulation successful! Global champion found on Continent " << global_champ_rank << endl;
        cout << "Check outputs/best_tour.dat for the final map sequence." << endl;


        // Cleanup: delete all "poplog_" FILES
        cout << "Cleaning up legacy poplog files..." << endl;
        for (const auto& entry : std::filesystem::directory_iterator("outputs")) {
            std::string current_file = entry.path().filename().string();
            
            // If the filename starts with "poplog_", delete it
            if (current_file.find("poplog_") == 0) {
                std::filesystem::remove(entry.path());
            }
        }
        cout << "Cleanup complete. Workspace is clean." << endl;

    } else if (rank == global_champ_rank) {
        // If this rank won, send the tour array over to Rank 0
        vector<int> my_tour = population.GetBestTour();
        MPI_Send(my_tour.data(), my_tour.size(), MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    
    // Closes MPI environment
    MPI_Finalize();

    return 0;
}