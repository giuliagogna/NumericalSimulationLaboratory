#include "../../utils/random/random.h"
#include "../../utils/auxiliary_functions/functions.h"
#include "../../utils/data_blocking/data_blocking.h"
#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

int main(){

    Random rnd = Random();
    InitializeGenerator(rnd);

    // Setting the parameters for the Buffon experiment

    double d = 10;   // Distance between the lines
    double l = 5;    // Length of the needle
    int N = 100;     // Number of blocks
    int L = 10000;   // Number of throws for the calculation in each block

    ofstream pi_values;
    OpenOutputFile(pi_values, "01_3_pi_estimate.dat");
    pi_values << "# Throws Blocks pi_estimate error" << endl;

    // The data blocker is initialized with block length of 1, since the values passed are already
    // blocked estimates of pi, since it is not possible to estimate pi from a single throw
    DataBlocker blocker(1);

    // Logic: 
    //      - for each throw I extract a random number between 0 and d/2 (distance of the centre of the needle from the closest line)
    //      - I extract the coordinates of one end of the needle: angle needs to be uniformly distributed:
    //              -> I extract 2 uniformly distributed variables dx and dy between 0 and 1
    //              -> calculate the norm dx^2+dy^2
    //              -> if norm is <= 1 I accept the throw
    //         This samples the unit circle uniformlym and therefore the accepted points have an angle that is uniformly distributed
    //      - I calculate the norm of (dx, dy)
    //      - I compute the sine of the angle between the needle and the lines as dy/norm 
    //      - If the distance of the centre of the needle from the closest line is smaller than l/2*sin(theta) then I count a crossing.

    for(int i = 0; i < N; i++){ // Loop over the blocks

        int count_crossed = 0; // Counter for the number of times the needle crosses a line

        for (int j = 0; j < L; j++){ // Loop over the throws

            double d_centre = rnd.Rannyu(0, d/2.0); // Distance of the centre of the needle from the closest line

            double dx = rnd.Rannyu();           // x coordinate of one end of the needle
            double dy = rnd.Rannyu();           // y coordinate of one end of the needle
            double norm = sqrt(dx*dx + dy*dy);  // Norm of the vector (dx, dy)

            while(norm > 1){ // Accepting only points that are inside the unit circle
                dx = rnd.Rannyu();
                dy = rnd.Rannyu();
                norm = sqrt(dx*dx + dy*dy);
            }

            double sin_theta = dy / norm; // Sine of the angle between the needle and the lines

            if(d_centre <= (l/2)*sin_theta){
                count_crossed++;
            }

        }

        // Estimating pi in the current block
        double pi_block = (2.0 * l * L) / (d * count_crossed);
        blocker.add_measurement(pi_block);

        // Since the internal block size is 1, a block is completed immediately
        pi_values << (i + 1) * L << " "
                  << blocker.get_completed_blocks() << " "
                  << blocker.get_mean() << " " 
                  << blocker.get_error()
                  << endl;
        
    }

    pi_values.close();

    rnd.SaveSeed();


    return 0;
}