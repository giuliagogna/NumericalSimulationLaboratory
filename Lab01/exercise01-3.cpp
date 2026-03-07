#include "random/random.h"
#include "auxiliary_functions/functions.h"
#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

int main(){

    Random rnd = Random();
    InitializeGenerator(rnd);

    // Setting the parameters for the Buffon experiment

    double d = 10; // Distance between the lines
    double L = 5; // Length of the needle
    int M = 100; // Number of blocks
    int N = 1000; // Number of throws in each block

    ofstream pi_values;
    OpenOutputFile(pi_values, "01_3_pi_estimate.dat");
    pi_values << "# throws pi_estimate error" << endl;

    double current_progressive_mean = 0.0;
    double current_progressive_mean_squared = 0.0;

    // Logic: 
    //      - for each throw I extract a random number between 0 and d/2 (distance of the centre of the needle from the closest line)
    //      - I extract the coordinates of one end of the needle: angle needs to be uniformly distributed:
    //              -> I extract 2 uniformly distributed variables dx and dy between 0 and 1
    //              -> calculate the norm dx^2+dy^2
    //              -> if norm is <= 1 I accept the throw
    //         This samples the unit circle uniformlym and therefore the accepted points have an angle that is uniformly distributed
    //      - I calculate the norm of (dx, dy)
    //      - I compute the sine of the angle between the needle and the lines as dy/norm 
    //      - If the distance of the centre of the needle from the closest line is smaller than L/2*sin(theta) then I count a crossing.

    for(int i = 0; i < M; i++){

        int count_crossed = 0; // Counter for the number of times the needle crosses a line

        for (int j = 0; j < N; j++){

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

            if(d_centre <= (L/2)*sin_theta){
                count_crossed++;
            }

        }

        // Estimating pi in the current block
        double pi_block = (2.0 * L * N) / (d * count_crossed);
        double pi_block_squared = pi_block * pi_block;

        current_progressive_mean = (double(i) / double(i + 1)) * current_progressive_mean + pi_block / double(i + 1);
        current_progressive_mean_squared = (double(i) / double(i + 1)) * current_progressive_mean_squared + pi_block_squared / double(i + 1);

        // Number of current throws
        int throws = current_throws(i, N);

        pi_values << throws << " " 
                  << current_progressive_mean << " " 
                  << error(current_progressive_mean, current_progressive_mean_squared, i) 
                  << endl;

    }



    pi_values.close();

    rnd.SaveSeed();


    return 0;
}