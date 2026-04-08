#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#include "../../../utils/random/random.h"
#include "../../../utils/auxiliary_functions/functions.h"
#include "../../../utils/metropolis/metropolis.h"

using namespace std;

int main() {

    Random rnd;
    InitializeGenerator(rnd);

    vector<string> dist_types = {"uniform", "gaussian"};

    int N_steps = 500000; 

    Psi100 psi_1s;                      
    Psi210 psi_2p;

    for (const string& dist_type : dist_types){

        string suffix = (dist_type == "uniform") ? "U" : "G";

        ofstream output_1s_r, output_2p_r;

        string filename1s = "autocorr/autocorr_r100_" + suffix + ".dat";
        string filename2p = "autocorr/autocorr_r210_" + suffix + ".dat";
        
        OpenOutputFile(output_1s_r, filename1s);
        output_1s_r << "# r" << endl;

        OpenOutputFile(output_2p_r, filename2p);
        output_2p_r << "# r" << endl;

        Position pos1s = {1.0, 1.0, 1.0};
        Position pos2p = {1.0, 1.0, 1.0};
        
        MetropolisAlgorithm<Psi100> metropolis100(psi_1s, pos1s, dist_type);
        MetropolisAlgorithm<Psi210> metropolis210(psi_2p, pos2p, dist_type);

        metropolis100.tune_step(pos1s, rnd, 1.0);
        metropolis210.tune_step(pos2p, rnd, 1.0);

        metropolis100.reset_position(pos1s); 
        metropolis210.reset_position(pos2p);

        cout << "Generating raw data..." << endl;
        for(int i = 0; i < N_steps; i++) {
            metropolis100.perform_metropolis_move(pos1s, rnd);
            double r1s = sqrt(pos1s.x*pos1s.x + pos1s.y*pos1s.y + pos1s.z*pos1s.z);
            output_1s_r << r1s << endl;

            metropolis210.perform_metropolis_move(pos2p, rnd);
            double r2p = sqrt(pos2p.x*pos2p.x + pos2p.y*pos2p.y + pos2p.z*pos2p.z);
            output_2p_r << r2p << endl;
        }

        cout << "Data saved in " << filename1s << ", " << filename2p << endl;

    }

    return 0;

}