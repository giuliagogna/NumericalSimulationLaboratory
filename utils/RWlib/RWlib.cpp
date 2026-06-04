#include "RWlib.h"
#include "../random/random.h"

#include <cmath>

void step_lattice(Position& p, Random& rnd, double step_length) {
    // Extract a random number between 0 and 5 to determine the direction of the step
    int direction = int(rnd.Rannyu() * 6); 

    // Update position based on the chosen direction
    if (direction == 0) p.x += step_length;
    else if (direction == 1) p.x -= step_length;
    else if (direction == 2) p.y += step_length;
    else if (direction == 3) p.y -= step_length;
    else if (direction == 4) p.z += step_length;
    else if (direction == 5) p.z -= step_length; 
}

// This function uses the uniform sampling of the solid angle with the inverse transform method 
void step_continuum(Position& p, Random& rnd, double step_length){

    double phi = rnd.Rannyu(0, 2*M_PI);
    double theta = rnd.distr_sin();

    double dx = step_length * sin(theta) * cos(phi);
    double dy = step_length * sin(theta) * sin(phi);
    double dz = step_length * cos(theta);

    p.x += dx;
    p.y += dy;
    p.z += dz;
}