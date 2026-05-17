#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <string>
#include <iostream>
#include <fstream>

#include "GenerateCoords.h"
#include "../../utils/auxiliary_functions/functions.h"
#include "mpi.h"

using namespace std;
using namespace arma;

struct Config {
    int N_CITIES = 0;
    int POP_SIZE = 0;
    int N_GENERATIONS = 0;
    string GENERATE_COORDS = "False";
    string COORDS_TYPE = "";
    int N_MIGR = 0; // Number of generations between migrations (only relevant for parallel GA)
};

inline Config ReadInput(const string& filename) {
    Config config;
    ifstream in_file(filename);

    if (!in_file.is_open()) {
        cerr << "ERROR: Unable to open input file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    string key;
    // Loop reads the file word-by-word until it hits ENDINPUT
    while (in_file >> key && key != string("ENDINPUT")) {
        if (key == "N_CITIES") {
            in_file >> config.N_CITIES;
        } else if (key == "POP_SIZE") {
            in_file >> config.POP_SIZE;
        } else if (key == "N_GENERATIONS") {
            in_file >> config.N_GENERATIONS;
        } else if (key == "GENERATE_COORDS") {
            in_file >> config.GENERATE_COORDS;
        } else if (key == "COORDS_TYPE") {
            in_file >> config.COORDS_TYPE;
        } else if (key == "N_MIGR") {
            in_file >> config.N_MIGR;
        }
        else {
            // If the key isn't any of the above, immediately crash and warn the user!
            throw invalid_argument("Input Parser Error: Unknown parameter '" + string(key) + "' found in " + filename);
        }
    }
    
    in_file.close();
    return config;
}


// Generate or read coordinates
inline mat SetupCoordinates(Config& config, int rank) {
    mat coord_mat;

    if (config.GENERATE_COORDS == "True") {

        coord_mat.set_size(config.N_CITIES, 2);

        if (rank == 0) { // Only node 0 generates the coordinates, so all the nodes work on the same set of coords
            cout << "Generating " << config.N_CITIES << " coordinates (" << config.COORDS_TYPE << ")...\n";
            
            GenerateCoords gen_coords(config.N_CITIES); 
            
            if (config.COORDS_TYPE == "Circular") {
                gen_coords.GenCirc();
            } else if (config.COORDS_TYPE == "Square") {
                gen_coords.GenSquare();
            } else {
                throw std::invalid_argument("Coordinate Setup Error: Unknown COORDS_TYPE in input file.");
            }
            
            gen_coords.save_coords(); // Rank 0 saves a copy to disk for your plots later
            coord_mat = gen_coords.get_coords_matrix(); 
        }

        // Broadcast the information to everyone
        MPI_Bcast(coord_mat.memptr(), config.N_CITIES * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
    } else {

        if (rank == 0) cout << "Reading coordinates from input file...\n";

        if (config.COORDS_TYPE == "Square"){

            if (!coord_mat.load("input/Coords_square.dat", arma::raw_ascii)) {
                throw runtime_error("Coordinate Setup Error: Unable to load input/Coords_square.dat");
            }

        } else if (config.COORDS_TYPE == "Circular") {

            if (!coord_mat.load("input/Coords_circumference.dat", arma::raw_ascii)) {
                throw runtime_error("Coordinate Setup Error: Unable to load input/Coords_circumference.dat");
            }

        } else if (config.COORDS_TYPE == "Italy") {

            if (!coord_mat.load("input/cap_prov_ita.dat", arma::raw_ascii)) {
                throw runtime_error("Coordinate Setup Error: Unable to load input/cap_prov_ita.dat");
            }
        } else {
            // Catch typos in the input.dat file!
            throw invalid_argument("Coordinate Setup Error: Unknown COORDS_TYPE to read.");
        }

        config.N_CITIES = coord_mat.n_rows;

    }
    

    return coord_mat;
}


// Added rank and size as parameters
inline string SetupLoggers(ofstream& pop_log, ofstream& output_log, const Config& config, int rank, int size) {
    
    // Generate the filename on all ranks so the function can return it safely
    string filename = "poplog_N" + to_string(config.N_CITIES) + 
                      "_Pop" + to_string(config.POP_SIZE) + 
                      "_Gen" + to_string(config.N_GENERATIONS) + 
                      "_" + config.COORDS_TYPE + ".dat";
                      
    // Only rank 0 has the right to write on memory
    if (rank == 0) {
        
        // =========================================================
        // SETUP THE MASTER DATA LOG
        // =========================================================
        OpenOutputFile(pop_log, filename);

        pop_log << "# --- TSP Genetic Algorithm Run ---\n"
                << "# Number of Cities: " << config.N_CITIES << "\n"
                << "# Population Size: " << config.POP_SIZE << "\n"
                << "# Total Generations: " << config.N_GENERATIONS << "\n"
                << "# Map Shape: " << config.COORDS_TYPE << "\n"
                << "# Continents (MPI): " << size << "\n"
                << "# ---------------------------------\n"
                << "Generation Rank Fitness ";
                
        for(int j = 0; j < config.N_CITIES + 1; j++) {
            pop_log << "City_" << j+1 << " ";
        }
        pop_log << endl;

        // =========================================================
        // SETUP THE SUMMARY REPORT (output.dat)
        // =========================================================
        OpenOutputFile(output_log, "output.dat");

        output_log << "=========================================\n";
        output_log << "       TSP SIMULATION SUMMARY            \n";
        output_log << "=========================================\n";
        output_log << "Number of Cities:    " << config.N_CITIES << "\n";
        output_log << "Population Size:     " << config.POP_SIZE << "\n";
        output_log << "Total Generations:   " << config.N_GENERATIONS << "\n";
        output_log << "Map Shape:           " << config.COORDS_TYPE << "\n";
        output_log << "Number of Continents:" << size << "\n"; // Added MPI info!
        output_log << "Migration Interval:  " << config.N_MIGR << "\n"; // Added Migration info!
        output_log << "=========================================\n\n";
        output_log << "Generation | Best Fitness\n";
        output_log << "-----------------------------------------\n";
    }

    return filename;
}


#endif