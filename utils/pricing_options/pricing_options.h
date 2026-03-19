#ifndef PRICING_OPTIONS_H
#define PRICING_OPTIONS_H

#include <vector>

using namespace std;

double N(double x);
vector<double> BlackScholes(double S0, double K, double T, double r, double sigma);


#endif