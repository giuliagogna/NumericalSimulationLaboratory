#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../../utils/random/random.h"
#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/data_blocking/data_blocking.h"

using namespace std;

const string directory = "Lab01";

int main(){

    // Setting up the random number generator
    Random rnd;
    InitializeGenerator(rnd);

    int M = 100000;      // Number of random numbers to generate
    int N = 100;         // Number of blocks
    int L = M/N;         // Number of throws in each block

    // Setting up the DataBlocking
    DataBlocker blocker_mean(L);
    DataBlocker blocker_variance(L);

    // ==============================================================================================
    // 01.1.1 & 01.1.2 - Means, Variances and their uncertainties
    // ==============================================================================================

    // Opens the files to save the progressive means, variances and their respective uncertainties
    ofstream results_output;
    OpenOutputFile(results_output, "01_1_means_and_variances.dat");
    results_output << "# Throws Block Mean Error_Mean Variance Error_Variance" << endl; // Header for the output file

    for(int i=0; i<M; i++){

        double r = rnd.Rannyu();

        blocker_mean.add_measurement(r);
        blocker_variance.add_measurement((r-0.5)*(r-0.5));

        if((i+1)%L == 0 && i!=0){
            // Save the current number of throws and the corresponding progressive mean and variances and their respective uncertainties to file
            results_output << blocker_mean.get_current_throws() << " "
                           << blocker_mean.get_completed_blocks() << " " 
                           << blocker_mean.get_mean() << " " 
                           << blocker_mean.get_error() << " "
                           << blocker_variance.get_mean() << " " 
                           << blocker_variance.get_error() 
                           << endl;

        }

    }

    
    // ==============================================================================================
    // 01.1.3 - Chi-squared test
    // ==============================================================================================
    
    int n_bins = 100;                            // Number of bins for the chi-squared test
    int n_throws = 10000;                        // Number of throws for each chi-squared test
    double p = double(n_throws)/double(n_bins);  // Expected number of throws in each bin

    int n_chi_squared = 100000;                     // Number of chi-squared tests to perform

    ofstream chi_squared_output;
    OpenOutputFile(chi_squared_output, "01_1_chi_squared.dat");
    chi_squared_output << "# Chi_Squared_Value" << endl; // Header for the output file

    vector<int> bins(n_bins); // Allocate the vector only once

    for(int i=0; i<n_chi_squared; i++){

        fill(bins.begin(), bins.end(), 0);  // Initialize bins to zero

        for(int j=0; j<n_throws; j++){
            double r = rnd.Rannyu();
            int bin_index = int(r*n_bins);  // Determine which bin the random number falls into
            bins[bin_index]++;
        }

        // Calculate the chi-squared value for this test
        double chi_squared = 0.0;
        for(int k=0; k<n_bins; k++){
            chi_squared += (bins[k] - p) * (bins[k] - p) / p;
        }

        chi_squared_output << chi_squared << endl;
    }

    rnd.SaveSeed();

    results_output.close();
    chi_squared_output.close();

    return 0;
}