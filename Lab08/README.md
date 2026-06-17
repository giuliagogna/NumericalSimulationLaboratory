# Laboratory 08 - Variational Monte Carlo and Simulated Annealing

This directory contains the C++ implementation and data analysis for the eighth laboratory session, which focuses on estimating the ground state energy of a system with a non-analytically solvable potential. It utilizes a Variational Monte Carlo (VMC) approach combined with a Simulated Annealing (SA) algorithm to optimize the parameters of a trial wave function, minimizing the energy. Subsequently, it performs a high-statistics evaluation of the system's energy using the optimized wave function and compares the results of the Monte Carlo sampling with the numerical solution obtained via the central differences matrix method.

## Project Structure
- `src/simulated_annealing.h`: Header file containing the energy calculation, the evaluation of the Hamiltonian's expectation value, and the Simulated Annealing algorithm.
- `src/optimize.cpp`: Source file for optimizing the two variational parameters using the Simulated Annealing algorithm.
- `src/production.cpp`: Source file for evaluating the final energy of the optimized wave function.
- `inputs/config.txt`: Configuration file storing the simulation parameters.
- `inputs/optimal_params.txt`: File storing the final optimized parameters.
- `../utils/metropolis/`: Object-oriented implementation of the Metropolis algorithm, featuring automated step tuning and equilibration.
- `../utils/data_blocking/`: Object-oriented implementation of the Data Blocking method for statistical uncertainty estimation.
- `../utils/random/`: Specialized library for random number generation (provided).
- `../utils/auxiliary_functions/`: Custom helper functions.
- `analysis8.ipynb`: Jupyter Notebook containing the main data analysis, including parameter space trajectories and the superposition of sampled and analytical wave functions.
- `Makefile`: Automated build system for compilation and execution.

## Compilation and Usage
The project uses a hidden `.build/` directory to store object files and executables, keeping the root directory clean. 

**Important:** The main simulation dynamically reads its execution parameters from `inputs/config.txt`. Make sure to modify these parameters if needed before running the code.

### 1. Build
To compile the C++ exercise, run the following command while in the `Lab08/` directory:

```bash
    make
```

### 2. Execution

**Optimization**
To run the optimization process, from the `Lab08/` directory, run:

```bash
    make run_opt
```

**Production**
To run the production code, from the `Lab08/` directory, run:

```bash
    make run_prod
```

## Clean Up
To remove the hidden `.build/` directory and generated seed files, run:

```bash
    make clean
```

## Data Analysis
The simulation results are processed in the `analysis8.ipynb` notebook. The analysis displays the evolution of the energy throughout the SA execution and the trajectory of the parameters within the parameter space. Subsequently, it presents the progressive mean and statistical uncertainty of the energy calculated with the optimized wave function. 

Finally, it plots the histogram of the spatial configurations sampled by the Metropolis algorithm during the evaluation of the Hamiltonian's expectation value. This sampled trial distribution is superimposed onto its exact analytical form for direct comparison. The true ground state wave function is also calculated numerically using the central differences method (matrix diagonalization). As expected from the Variational Principle, the analysis confirms that the optimized energy found via Simulated Annealing effectively acts as an upper bound to the "true" ground state energy obtained with the numerical matrix method.