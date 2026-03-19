#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../../utils/random/random.h"
#include "../../utils/auxiliary_functions/functions.h"

using namespace std;

const string directory = "Lab01";

int main(){

    // Setting up the random number generator
    Random rnd;
    InitializeGenerator(rnd);

    int M = 100000;      // Number of random numbers to generate
    int N = 100;         // Number of blocks
    int L = M/N;         // Number of throws in each block


    // ==============================================================================================
    // 01.1.1 & 01.1.2 - Means, Variances and their uncertainties
    // ==============================================================================================

    // Opens the files to save the progressive means, variances and their respective uncertainties
    ofstream results_output;
    OpenOutputFile(results_output, "01_1_means_and_variances.dat");
    results_output << "# Throws Mean Error_Mean Variance Error_Variance" << endl; // Header for the output file

    double current_progressive_mean = 0.0;
    double current_progressive_mean_squared = 0.0;

    double current_progressive_variance = 0.0;
    double current_progressive_variance_squared = 0.0;

    for(int i=0; i<N; i++){
        
        // Calculates the mean and the mean square, variance and variance squared in the current block
        double sum_mean_block = 0.0;
        double sum_var_block = 0.0;
        
        
        for(int j=0; j<L; j++){
            double r = rnd.Rannyu();
            sum_mean_block += r;
            sum_var_block += (r - 0.5) * (r - 0.5);
        }


        // Calculates the mean and the mean square in the current block
        double current_mean = sum_mean_block / L;
        double current_mean_squared = current_mean * current_mean;

        // Stores the progressive mean and the progressive mean square up to the current block
        current_progressive_mean = (double(i)/(i+1)) * current_progressive_mean + current_mean/(i+1);
        current_progressive_mean_squared = (double(i)/(i+1)) * current_progressive_mean_squared + current_mean_squared/(i+1);  
        
        // Calculates the variance and the variance squared in the current block
        double current_variance = sum_var_block / L;
        double current_variance_squared = current_variance * current_variance;
        
        // Stores the progressive variance and the progressive variance square up to the current block
        current_progressive_variance = (double(i)/(i+1)) * current_progressive_variance + current_variance/(i+1);
        current_progressive_variance_squared = (double(i)/(i+1)) * current_progressive_variance_squared + current_variance_squared/(i+1);

        // Save the current number of throws and the corresponding progressive mean and variances and their respective uncertainties to file
        results_output << current_throws(i, L) << " " 
                       << current_progressive_mean << " " 
                       << error(current_progressive_mean, current_progressive_mean_squared, i) << " "
                       << current_progressive_variance << " " 
                       << error(current_progressive_variance, current_progressive_variance_squared, i) 
                       << endl;
    }


    
    // ==============================================================================================
    // 01.1.3 - Chi-squared test
    // ==============================================================================================
    
    int n_bins = 100;                            // Number of bins for the chi-squared test
    int n_throws = 10000;                        // Number of throws for each chi-squared test
    double p = double(n_throws)/double(n_bins);  // Expected number of throws in each bin

    int n_chi_squared = 10000;                     // Number of chi-squared tests to perform

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