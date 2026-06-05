// CALCULATION OF THE PRICE OF A EUROPEAN CALL AND PUT OPTION USING THE DIRECT SAMPLING METHOD

#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/random/random.h"
#include "../../utils/pricing_options/pricing_options.h"
#include "../../utils/data_blocking/data_blocking.h"

#include <iostream>
#include <fstream>

using namespace std;

int main() {

    Random rnd;
    InitializeGenerator(rnd);

    int M = 1000000;   // Total number of throws
    int N = 100;       // Number of blocks
    int L = M/N;       // Number of throws in each block

    ofstream output;
    OpenOutputFile(output, "output_direct_03.dat");
    output << "#Throws Blocks Call_price Error_Call Put_Price Error_Price" << endl;

    // Data of the GBM process
    double S0 = 100.0;     // Asset price
    double T = 1.0;        // Delivery time
    double K = 100.0;      // Strike price
    double r = 0.1;        // Risk-free interest rate
    double sigma = 0.25;   // Volatility

    DataBlocker blocker_call(L);
    DataBlocker blocker_put(L);

    for(int i=0; i<M; i++){

        double Z = rnd.Gauss(0, 1);
        double S = S0 * exp((r-0.5*sigma*sigma)*T + sigma*Z*sqrt(T));
        blocker_call.add_measurement(exp(-r*T) * max(0.0, S-K));
        blocker_put.add_measurement(exp(-r*T) * max(0.0, K-S));

        if((i+1)%L==0 && i!=0){

            output << blocker_call.get_current_throws() << " " 
                   << blocker_call.get_completed_blocks() << " "
                   << blocker_call.get_mean() << " "
                   << blocker_call.get_error() << " "
                   << blocker_put.get_mean() << " "
                   << blocker_put.get_error() << " "
                   << endl;

        }

    }

    output.close(); 

    return 0;
}