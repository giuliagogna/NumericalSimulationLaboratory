#include "../../utils/random/random.h"
#include "../../utils/auxiliary_functions/functions.h"
#include <iostream>
#include <fstream>
#include <vector>


int main(){

    Random rnd;
    InitializeGenerator(rnd);

    // Instantiate a vector with the numeber of throws in each experiment
    vector<int> throws = {1, 2, 10, 100};
    int M = 10000; // Total number of throws for each experiment

    // Open the output files for the three different distributions
    // Uniform distribution
    ofstream output_uniform;
    OpenOutputFile(output_uniform, "01_2_random_uniform.dat");
    output_uniform << "# N=1 N=2 N=10 N=100" << endl;

    // Exponential distribution
    double lambda = 1.0; // Parameter for the exponential distribution
    ofstream output_exponential;
    OpenOutputFile(output_exponential, "01_2_random_exponential.dat");
    output_exponential << "# N=1 N=2 N=10 N=100" << endl;

    // Lorentzian distribution
    double mu = 0.0;    // Location parameter for the Lorentzian distribution
    double gamma = 1.0; // Scale parameter for the Lorentzian distribution
    ofstream output_Lorentzian;
    OpenOutputFile(output_Lorentzian, "01_2_random_Lorentzian.dat");
    output_Lorentzian << "# N=1 N=2 N=10 N=100" << endl;

    for(int i = 0; i < M; i++){

        for(int j = 0; j < throws.size(); j++){

            double sum_uniform = 0.0;
            double sum_exponential = 0.0;
            double sum_Lorentzian = 0.0;    

            int n_throws = throws[j];

            for(int k=0; k<n_throws; k++){
                sum_uniform += rnd.Rannyu();
                sum_exponential += rnd.Exp(lambda);
                sum_Lorentzian += rnd.Lorentz(mu, gamma);
            }

            output_uniform << sum_uniform / double(n_throws) << " ";
            output_exponential << sum_exponential / double(n_throws) << " ";
            output_Lorentzian << sum_Lorentzian / double(n_throws) << " ";

        }

        output_uniform << endl;
        output_exponential << endl;
        output_Lorentzian << endl;

    }

    output_uniform.close();
    output_exponential.close();
    output_Lorentzian.close();

    rnd.SaveSeed();

    return 0;
}