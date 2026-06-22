# Laboratory 10 - Parallel Genetic Algorithm and the Traveling Salesman Problem

This directory contains the C++ implementation and data analysis for the tenth laboratory session, which focuses on solving the Traveling Salesman Problem (TSP) using a Parallel Genetic Algorithm via the Message Passing Interface (MPI). Building upon the sequential architecture developed in Laboratory 09, this project utilizes an "Island Model": multiple independent populations (continents) evolve simultaneously on separate processing nodes. To prevent premature convergence to local minima, the continents periodically exchange their absolute best individuals via a synchronous migration protocol, dramatically increasing the exploration of the geometric phase space.

## Project Structure
- `src/Individual.h` and `src/Individual.cpp`: Class representing a single candidate solution (chromosome). It encapsulates the 1D vector of city indices, enforces TSP boundary conditions (starting and ending at city 1), and computes the fitness (route length).
- `src/Population.h` and `src/Population.cpp`: The core evolutionary engine. It manages the collection of individuals and executes the evolutionary cycle, including rank-based probabilistic selection, Elitism, 1D crossover, and four specialized mutation operators (Pair Permutation, Shift, Inversion, and Block Swap). It now includes methods to seamlessly integrate foreign immigrants.
- `src/Initializer.h`: Header containing abstraction functions for reading configuration files, and setting up the Armadillo coordinate matrices and initializing unique random number generator seeds for each MPI rank.
- `src/exercise10.cpp`: The main MPI driver script. It initializes the distributed memory environment, manages the migration protocol via `MPI_Sendrecv`, reduces the global fitness data, and handles the optimized I/O operations strictly through Rank 0.
- `inputs/input.dat`: Configuration file storing hyperparameters such as `POP_SIZE`, `N_GENERATIONS`, `N_CITIES`, spatial distribution types and the parallel parameter `N_MIGR` (migration interval).
- `outputs/`: Directory designated for the reports (`output.dat` and `best_tour.dat`). Contains two subdirectories with the results used in the comparison with the results in Lab09 and one for the longer run to refine the results.
- `../utils/random/`: Specialized library for random number generation (provided).
- `../utils/auxiliary_functions/`: Custom helper functions utilized throughout the project.
- `analysis10.ipynb`: Jupyter Notebook containing the main data analysis, including fitness convergence tracking and geographic visualizations of the best tour.
- `Makefile`: Automated build system for compilation and execution.

## Compilation and Usage
The project uses a hidden `.build/` directory to store object files and executables, keeping the root directory clean.

**Important:** The simulation heavily relies on the Armadillo C++ linear algebra library for efficient coordinate matrix handling. Ensure Armadillo is installed and properly linked in your environment. 

**Important:** The algorithm must be compiled and executed using the MPI environment OpenMPI.

**Important:** The simulation dynamically reads its execution parameters from `inputs/input.dat`. Make sure to modify these parameters if needed before running the code.

**Important:** The main simulation dynamically reads its execution parameters from `inputs/config.txt`. Make sure to modify these parameters if needed before running the code.

### 1. Build
To compile the C++ exercise, run the following command while in the `Lab10/` directory:

```bash
    make
```

### 2. Execution

To run the standard Genetic Algorithm simulation, from the `Lab10/` directory, run:

```bash
    make run
```

This executes the command

```bash
    time mpiexec -np 7 ./$(BUILD)/exercise10.exe
```

If you want to run the code on more than 7 cores, just find this line ('time mpiexec -np 7 ./$(BUILD)/exercise10.exe') in the `Makefile` and change the 7 with the number of cores you desire to run the code on.

## Clean Up
To remove the hidden `.build/` directory and generated seed files, run:

```bash
    make clean
```

## Data Analysis
The simulation results are processed in the `analysis10.ipynb` notebook. The analysis tracks the algorithmic convergence by plotting the absolute best global fitness found across the entire cluster against the number of generations. This demonstrates how the combined exploration of the distributed nodes quickly locates highly optimized regions, and how the migration protocol ensures superior traits rapidly propagate to the entire cluster.

The notebook visually traces the final optimal closed loop on the 2D spatial atlas of the 110 Italian provincial capitals. The analysis highlights the performance leap provided by the Island Model: by scaling the total effective population and avoiding the local minima traps that bottlenecked the sequential algorithm, the parallel architecture achieves highly competitive, global-scale optimization in a fraction of the time.

_**Note on Data Optimization and Logs:** In Laboratory 09, large "synthetic" log files were saved to manage the massive data output of the populations without exceeding GitHub's file size limits. In Laboratory 10, this approach is no more adopted. To maximize cluster performance and prevent network bottlenecks, disk I/O has been entirely redesigned. The individual continent poplog_ _files are no longer saved. Instead, Rank 0 acts as a central coordinator, evaluating the cluster in memory via MPI_Reduce and MPI_Allreduce, and writing exactly two lightweight files: `output.dat` (the global fitness history) and `best_tour.dat` (the final optimal map). An automated C++ cleanup routine runs before termination to ensure any other logs are deleted, keeping the repository permanently clean and lightweight._