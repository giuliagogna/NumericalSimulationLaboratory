#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/random/random.h"
#include "../../utils/data_blocking/data_blocking.h"

#include <iostream>

using namespace std;

double integrand(double x){
    return M_PI/2.0*cos(M_PI*x/2.0);
};

double importance_integrand(double x){
    // This function gives the new integrand to be used for importance sampling, which is the original integrand divided by the distribution used for importance sampling d(x) = 2*(1-x)
    return M_PI/4.0*cos(M_PI*x/2.0)/(1-x);
};

int main(){

    Random rnd;
    InitializeGenerator(rnd);   

    ofstream output;
    OpenOutputFile(output, "02_1_integral.dat");
    output << "# Throws Blocks Uniform_Integral Uniform_Integral_Error Importance_Integral Importance_Integral_Error" << endl; // Header for the output file

    int M = 1000000; // Total number of random points
    int N = 100;     // Number of blocks
    int L = M/N;     // Number of points in each block

    DataBlocker blocker_uniform(L);
    DataBlocker blocker_importance(L);

    for(int i=0; i<M; i++){

        double r = rnd.Rannyu();
        double eval_integrand_uniform = integrand(r);
        blocker_uniform.add_measurement(eval_integrand_uniform);

        double eval_integrand_importance = importance_integrand(rnd.distr_importance());
        blocker_importance.add_measurement(eval_integrand_importance);

        if((i+1)%L==0 && i!=0){

            output << blocker_uniform.get_current_throws() << " "
                   << blocker_uniform.get_completed_blocks() << " "
                   << blocker_uniform.get_mean() << " "
                   << blocker_uniform.get_error() << " "
                   << blocker_importance.get_mean() << " "
                   << blocker_importance.get_error()
                   << endl;

        }

    }

    output.close();

    return 0;
}