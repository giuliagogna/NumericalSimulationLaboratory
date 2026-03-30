// CALCULATION OF THE PRICE OF A EUROPEAN CALL AND PUT OPTION USING THE DIRECT SAMPLING METHOD


#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/random/random.h"
#include "../../utils/pricing_options/pricing_options.h"

#include <iostream>
#include <fstream>

using namespace std;

int main() {

    Random rnd;
    InitializeGenerator(rnd);

    int M = 1000000;   // Total number of throws
    int N = 100;       // Number of blocks
    int L = M/N;       // Number of throws in each block

    ofstream output_direct;
    OpenOutputFile(output_direct, "output_direct_03.dat");
    output_direct << "#Throws Call_price Error_Call Put_Price Error_Price" << endl;

    // Data of the GBM process
    double S0 = 100.0;     // Asset price
    double T = 1.0;        // Delivery time
    double K = 100.0;      // Strike price
    double r = 0.1;        // Risk-free interest rate
    double sigma = 0.25;   // Volatility

    double current_progressive_call = 0.0;
    double current_progressive_call_squared = 0.0;

    double current_progressive_put = 0.0;
    double current_progressive_put_squared = 0.0;

    double call_error = 0.0;
    double put_error = 0.0;


    for (int i=0; i<N; i++){

        double call_sum = 0.0;
        double put_sum = 0.0;

        for (int j=0; j<L; j++){
            double Z = rnd.Gauss(0, 1);
            double S = S0 * exp((r-0.5*sigma*sigma)*T + sigma*Z*sqrt(T));
            call_sum += exp(-r*T) * max(0.0, S-K);
            put_sum += exp(-r*T) * max(0.0, K-S);
        }

        double current_call = call_sum / L;
        double current_call_squared = current_call * current_call;

        double current_put = put_sum / L;
        double current_put_squared = current_put * current_put;

        current_progressive_call = (double(i)/(i+1)) * current_progressive_call + current_call/double(i+1);
        current_progressive_call_squared = (double(i)/(i+1)) * current_progressive_call_squared + current_call_squared/double(i+1);

        current_progressive_put = (double(i)/(i+1)) * current_progressive_put + current_put/double(i+1);
        current_progressive_put_squared = (double(i)/(i+1)) * current_progressive_put_squared + current_put_squared/double(i+1);

        call_error = error(current_progressive_call, current_progressive_call_squared, i);
        put_error = error(current_progressive_put, current_progressive_put_squared, i);

        output_direct << current_throws(i, L) << " " 
                      << current_progressive_call << " "
                      << call_error << " " 
                      << current_progressive_put << " " 
                      << put_error << endl;

    }

    cout << "Call price simulated = " << current_progressive_call << " +/- " << call_error << endl;
    cout << "Call price expected = " << BlackScholes(S0, K, T, r, sigma)[0] << endl;

    cout << "Put price simulated = " << current_progressive_put << " +/- " << put_error << endl;
    cout << "Put price expected = " << BlackScholes(S0, K, T, r, sigma)[1] << endl;

    output_direct.close();

    return 0;
}