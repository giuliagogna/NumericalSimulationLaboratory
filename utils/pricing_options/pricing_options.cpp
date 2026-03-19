#include "pricing_options.h"
#include <cmath>
#include <vector>
 
using namespace std;

double N(double x) {
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}

vector<double> BlackScholes(double S0, double K, double T, double r, double sigma){
    double d1 = 1./(sigma * sqrt(T)) * (log(S0 / K) + (r + (sigma*sigma) / 2.) * T);
    double d2 = d1 - sigma * sqrt(T);
    double C = S0 * N(d1) - K * exp(-r * T) * N(d2);
    double P = S0 *(N(d1) - 1.) - K * exp(-r * T) * (N(d2)-1.);

    return vector<double>{C, P};
}