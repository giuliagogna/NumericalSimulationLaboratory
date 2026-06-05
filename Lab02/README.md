# Laboratory 02 - Monte Carlo Integration and Random Walks

This directory contains the C++ implementation and data analysis for the second laboratory session, focusing on Monte Carlo integration techniques (Uniform and Importance Sampling) and the simulation of normal diffusion via 3D Random Walks.

## Project Structure

- `exercise02-1.cpp`, `exercise02-2.cpp`: main source files for the two exercises.
- `../utils/random/`: Specialized library for random number generation (provided).
- `../utils/data_blocking/`: Object-oriented implementation of the data blocking method for statistical uncertainty estimation.
- `../utils/RWlib/`: Custom library for simulating 3D Random Walks on discrete lattices and in continuous space.
- `../utils/auxiliary_functions/`: Custom helper functions.
- `analysis2.ipynb`: Jupyter Notebook containing the data analysis, curve fitting, and visualization.
- `Makefile`: automated build system for compilation and execution.

## Compilation and Usage

The project uses a hidden `.build/` directory to store object files and executables.

### Build
To compile all exercises at once:
```bash
make
```
### Execution
You can run the exercises individually using the dedicated make targets:

**Exercise 2.1 (Monte Carlo Integration):**
```bash
make run1
```

**Exercise 2.2 (3D Random Walks):**
```bash
make run2
```

Alternatively, to run all simulations in sequence:
```bash
make run
```

### Clean Up
To remove the hidden build directory and generated files:
```bash
make clean
```

## Data Analysis
The simulation results are processed in the `analysis2.ipynb` notebook. The analysis includes plotting the progressive estimates of the 1D integral and performing a square root fit on the random walk data to mathematically confirm normal diffusion ($f(N) = k\sqrt{N}$).