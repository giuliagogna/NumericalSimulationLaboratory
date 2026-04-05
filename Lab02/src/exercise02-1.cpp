#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/random/random.h"

#include <iostream>

using namespace std;

double integrand(double x){
    return M_PI/2.0*cos(M_PI*x/2.0);
};

double importance_integrand(double x){
    // This function gives the new integrand to be used for importance sampling, which is the original integrand divided by the distribution used for importance sampling d(x) = 2*(1-x)
    return M_PI/4.0*cos(M_PI*x/2.0)/(1-x);
};

//FIXME: use data blocking class instead of calculating the progressive mean and error by hand

int main(){

    Random rnd;
    InitializeGenerator(rnd);   

    ofstream output_uniform;
    OpenOutputFile(output_uniform, "02_1_integral.dat");
    output_uniform << "# Throws Uniform_Integral Uniform_Integral_Error Importance_Integral Importance_Integral_Error" << endl; // Header for the output file

    double low = 0.0;
    double high = 1.0;

    double interval_length = max(high, low) - min(high, low);

    int M = 1000000; // Total number of random points
    int N = 100;     // Number of blocks
    int L = M/N;     // Number of points in each block

    double current_progressive_estimate_uniform = 0.0;
    double current_progressive_estimate_uniform_squared = 0.0;

    double current_progressive_estimate_importance = 0.0;
    double current_progressive_estimate_importance_squared = 0.0;

    double eval_integrand = 0.0;

    for (int i=0; i<N; i++){

        // Calculates the integral estimate and its square, variance and variance squared in the current block
        double sum_uniform_block = 0.0;
        double sum_importance_block = 0.0;
        
        for(int j=0; j<L; j++){
            double r = rnd.Rannyu();

            // Evaluates the integrand at the random point and adds it to the sum for the current block using uniform sampling
            eval_integrand = integrand(r);
            sum_uniform_block += eval_integrand;

            // Evaluates the new integrand for importance sampling at the random point and adds it to the sum for the current block using importance sampling
            eval_integrand = importance_integrand(rnd.distr_importance(r));
            sum_importance_block += eval_integrand;
        }


        // Calculates the mean and the mean square in the current block
        double current_mean_uniform = sum_uniform_block / L;
        double current_mean_importance = sum_importance_block / L;

        // Calculates the integral estimate and its square in the current block
        double current_integral_estimate_uniform = interval_length * current_mean_uniform;
        double current_integral_estimate_squared_uniform = current_integral_estimate_uniform * current_integral_estimate_uniform;

        double current_integral_estimate_importance = interval_length * current_mean_importance;
        double current_integral_estimate_squared_importance = current_integral_estimate_importance * current_integral_estimate_importance;

        // Stores the progressive mean and the progressive mean square up to the current block
        current_progressive_estimate_uniform = ((double(i)/(i+1)) * current_progressive_estimate_uniform + current_integral_estimate_uniform/(i+1));
        current_progressive_estimate_uniform_squared = ((double(i)/(i+1)) * current_progressive_estimate_uniform_squared + current_integral_estimate_squared_uniform/(i+1)); 
        
        current_progressive_estimate_importance = ((double(i)/(i+1)) * current_progressive_estimate_importance + current_integral_estimate_importance/(i+1));
        current_progressive_estimate_importance_squared = ((double(i)/(i+1)) * current_progressive_estimate_importance_squared + current_integral_estimate_squared_importance/(i+1));


        output_uniform << current_throws(i, L) << " " 
                       << current_progressive_estimate_uniform << " " 
                       << error(current_progressive_estimate_uniform, current_progressive_estimate_uniform_squared, i) << " "
                       << current_progressive_estimate_importance << " "
                       << error(current_progressive_estimate_importance, current_progressive_estimate_importance_squared, i)
                       << endl;

    }

    output_uniform.close();

    return 0;
}