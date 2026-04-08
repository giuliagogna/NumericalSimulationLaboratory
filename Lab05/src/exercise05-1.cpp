#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "../../utils/random/random.h"
#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/metropolis/metropolis.h"
#include "../../utils/data_blocking/data_blocking.h"

using namespace std;

int main() {

    // INITIALIZING GENERATOR
    Random rnd;
    InitializeGenerator(rnd);

    // DATA BLOCKING CONFIGURATION

    string input_filename = "src/autocorrelation/config.txt"; 

    ifstream config(input_filename);
    if (!config) {
        cerr << "Error: Could not open configuration file: " << input_filename << endl;
        cerr << "Did you run the Python pipeline in autocorrelation/ first?" << endl;
        return 1;
    }

    string label;
    int n_equilibration_steps = 0;
    int L = 0;
    int n_blocks = 0;
    int N = 0;

    config >> label >> n_equilibration_steps;
    config >> label >> L;
    config >> label >> n_blocks;
    config >> label >> N;

    cout << "\n=== CONFIGURATION LOADED AUTOMATICALLY ===" << endl;
    cout << " - Equilibration steps: " << n_equilibration_steps << endl;
    cout << " - Block Length (L):    " << L << endl;
    cout << " - Number of Blocks:    " << n_blocks << endl;
    cout << " - Total Steps (N):     " << N << endl;
    cout << "==========================================\n" << endl;

    // INITIALIZE WAVEFUNCTIONS
    Psi100 psi_1s;                      
    Psi210 psi_2p;                      

    // Array of distributions to loop over
    vector<string> dist_types = {"uniform", "gaussian"};

    // =========================================================================
    // MAIN SIMULATION LOOP
    // =========================================================================
    for (const string& dist_type : dist_types) {
        
        cout << "\n============================================================" << endl;
        cout << "  Starting simulation with " << dist_type << " distribution" << endl;
        cout << "==============================================================\n" << endl;

        string suffix = (dist_type == "uniform") ? "U" : "G";

        // OPEN OUTPUT FILES (They will automatically close when the loop iteration ends)
        ofstream output_1s_r, output_1s_pos, output_2p_r, output_2p_pos;
        
        OpenOutputFile(output_1s_r, "05_1_r100_" + suffix + ".dat");
        output_1s_r << "# Block r100_Mean r100_Error" << endl;

        OpenOutputFile(output_1s_pos, "05_1_pos100_" + suffix + ".dat");
        output_1s_pos << "# x y z" << endl;

        OpenOutputFile(output_2p_r, "05_1_r210_" + suffix + ".dat");
        output_2p_r << "# Block r210_Mean r210_Error" << endl;

        OpenOutputFile(output_2p_pos, "05_1_pos210_" + suffix + ".dat");
        output_2p_pos << "# x y z" << endl;

        // INITIALIZE DATA BLOCKERS
        DataBlocker blocker1s(L);
        DataBlocker blocker2p(L);

        // INITIALIZE ALGORITHMS AND POSITIONS
        Position pos1s = {1.0, 1.0, 1.0};
        Position pos2p = {1.0, 1.0, 1.0};
        
        MetropolisAlgorithm<Psi100> metropolis100(psi_1s, pos1s, dist_type);
        MetropolisAlgorithm<Psi210> metropolis210(psi_2p, pos2p, dist_type);

        // TUNING

        metropolis100.tune_step(pos1s, rnd, 1.0);
        metropolis210.tune_step(pos2p, rnd, 1.5);

        cout << "Optimal step found for Psi100 (" << dist_type << "): " << metropolis100.get_step() << endl;
        cout << "Optimal step found for Psi210 (" << dist_type << "): " << metropolis210.get_step() << endl;

        // EQUILIBRATION
        metropolis100.reset_position(pos1s);
        metropolis210.reset_position(pos2p);

        metropolis100.equilibrate(pos1s, n_equilibration_steps, rnd);
        metropolis210.equilibrate(pos2p, n_equilibration_steps, rnd);

        // STARTING SIMULATION
        cout << "Starting Metropolis..." << endl;

        int prev_completed_1s = 0;
        int prev_completed_2p = 0;

        for(int i = 0; i < N; i++) {

            // Save positions
            output_1s_pos << pos1s.x << " " << pos1s.y << " " << pos1s.z << endl;
            output_2p_pos << pos2p.x << " " << pos2p.y << " " << pos2p.z << endl;

            // Perform moves
            metropolis100.perform_metropolis_move(pos1s, rnd);
            metropolis210.perform_metropolis_move(pos2p, rnd);
            
            // Measure radii and pass to blockers
            double r1s = sqrt(pos1s.x*pos1s.x + pos1s.y*pos1s.y + pos1s.z*pos1s.z);
            blocker1s.add_measurement(r1s);

            double r2p = sqrt(pos2p.x*pos2p.x + pos2p.y*pos2p.y + pos2p.z*pos2p.z);
            blocker2p.add_measurement(r2p);

            // Output blocking data if a new block is completed
            if(blocker1s.get_completed_blocks() > prev_completed_1s) {
                output_1s_r << blocker1s.get_completed_blocks() << " " 
                            << blocker1s.get_mean() << " " 
                            << blocker1s.get_error() << endl;
                prev_completed_1s = blocker1s.get_completed_blocks();
            }

            if(blocker2p.get_completed_blocks() > prev_completed_2p) {
                output_2p_r << blocker2p.get_completed_blocks() << " "
                            << blocker2p.get_mean() << " "
                            << blocker2p.get_error() << endl;
                prev_completed_2p = blocker2p.get_completed_blocks();
            }
        }

        cout << "Metropolis completed for " << dist_type << " distribution." << endl;
    } // End of distribution loop (files are auto-closed and objects destroyed here)

    cout << "\nALL SIMULATIONS COMPLETED SUCCESSFULLY!" << endl;

    return 0;
}