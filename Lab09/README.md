# Laboratory 09 - Genetic Algorithm and the Traveling Salesman Problem

This directory contains the C++ implementation and data analysis for the ninth laboratory session, which focuses on solving the Traveling Salesman Problem (TSP) using a Genetic Algorithm (GA). The project utilizes an object-oriented approach to evolve a population of candidate routes over successive generations, minimizing the total path length (evaluated using the $L^{(1)}$ cost function, defined as the sum of Euclidean distances). The algorithm is tested against standard geometric topologies (a circumference and a square) and scaled up to a real-world dataset of Italian provincial capitals to establish a computational baseline for future parallelization.

## Project Structure
- `src/Individual.h` and `src/Individual.cpp`: Class representing a single candidate solution (chromosome). It encapsulates the 1D vector of city indices, enforces TSP boundary conditions (starting and ending at city 1), and computes the fitness (route length).
- `src/Population.h` and `src/Population.cpp`: The core evolutionary engine. It manages the collection of individuals and executes the evolutionary cycle, including rank-based probabilistic selection, Elitism, 1D crossover, and four specialized mutation operators (Pair Permutation, Shift, Inversion, and Block Swap).
- `src/Initializer.h`: Header containing abstraction functions for reading configuration files, initializing the random number generator, and setting up the Armadillo coordinate matrices.
- `src/exercise09.cpp`: The streamlined main driver script that sets up the environment, initializes the population, and runs the generational loop while handling comprehensive data logging.
- `inputs/input.dat`: Configuration file storing hyperparameters such as POP_SIZE, N_GENERATIONS, N_CITIES, and spatial distribution types.
- `outputs/`: Directory designated for the master population logs and summary execution reports.
- `../utils/random/`: Specialized library for random number generation (provided).
- `../utils/auxiliary_functions/`: Custom helper functions utilized throughout the project.
- `analysis9.ipynb`: Jupyter Notebook containing the main data analysis, including fitness convergence tracking and geographic visualizations of the evolving tours.
- `Makefile`: Automated build system for compilation and execution.

## Compilation and Usage
The project uses a hidden `.build/` directory to store object files and executables, keeping the root directory clean.

**Important:** The simulation heavily relies on the Armadillo C++ linear algebra library for efficient coordinate matrix handling. Ensure Armadillo is installed and properly linked in your environment. 

**Important:** The main simulation dynamically reads its execution parameters from `inputs/input.dat`. Make sure to modify these parameters if needed before running the code.

**Important:** The main simulation dynamically reads its execution parameters from `inputs/config.txt`. Make sure to modify these parameters if needed before running the code.

### 1. Build
To compile the C++ exercise, run the following command while in the `Lab09/` directory:

```bash
    make
```

### 2. Execution

To run the standard Genetic Algorithm simulation, from the `Lab09/` directory, run:

```bash
    make run
```

## Clean Up
To remove the hidden `.build/` directory and generated seed files, run:

```bash
    make clean
```

## Data Analysis
The simulation results are processed in the `analysis9.ipynb` notebook. The analysis tracks the algorithmic convergence by plotting the fitness of the absolute best individual superimposed with the average fitness of the top 50% of the population across generations, proving that the population collectively optimizes rather than relying on isolated random mutations.

The notebook visually traces the evolution of the shortest path on a 2D spatial atlas, mapping the route from a randomized Generation 0 to the final optimized closed loop. The analysis confirms the absolute necessity of Elitism and sufficient population sizing to successfully navigate complex solution spaces.

Finally, the algorithm is applied to a map of 110 Italian provincial capitals. The analysis highlights that while the sequential algorithm is mathematically capable of finding highly optimized routes, the sheer scale of the solution space often traps it in local minima (evidenced by crossed paths in the visualization) or requires quite long execution times. This real-world application directly establishes the performance bottlenecks that will be addressed via parallelization in Laboratory 10.