#ifndef __RW__
#define __RW__

#include "../random/random.h"

struct Position {
    double x;
    double y;
    double z;
};

void step_lattice(Position& p, Random& rnd, double step_length);
void step_continuum(Position& p, Random& rnd, double step_length);

# endif // __RW__