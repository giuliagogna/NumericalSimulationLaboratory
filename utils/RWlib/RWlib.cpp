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

void step_continuum(Position& p, Random& rnd, double step_length) {
    double dx, dy, dz, norm;

    do {
        dx = rnd.Rannyu() * 2.0 - 1.0;
        dy = rnd.Rannyu() * 2.0 - 1.0;
        dz = rnd.Rannyu() * 2.0 - 1.0;
        
        norm = sqrt(dx*dx + dy*dy + dz*dz);
    } while (norm > 1.0); 

    p.x += step_length * (dx / norm);
    p.y += step_length * (dy / norm);
    p.z += step_length * (dz / norm);
    
}