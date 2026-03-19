#ifndef __RW__
#define __RW__

#include "../random/random.h"

struct Position {
    double x;
    double y;
    double z;
};

void step_lattice(Position& p, Random& rnd, double a);
void step_continuum(Position& p, Random& rnd, double a);

# endif // __RW__