# Laboratory 01 - Random Number Techniques and Buffon's Experiment

This directory contains the C++ implementation and data analysis for the first laboratory session, focusing on the Central Limit Theorem, probability distributions, and the Monte Carlo estimation of $\pi$.

## Project Structure

- `exercise01-1.cpp`, `exercise01-2.cpp`, `exercise01-3.cpp`: main source files for the three exercises.
- `../utils/random/`: Specialized library for random number generation (provided).
- `../utils/data_blocking/`: Object-oriented implementation of the data blocking method for statistical uncertainty estimation.
- `../utils/auxiliary_functions/`: Custom helper functions.
- `analysis1.ipynb`: Jupyter Notebook containing the data analysis and visualization.
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

**Exercise 1.1 (Mean and Variance):** 
```bash
make run1
```

**Exercise 1.2 (Central Limit Theorem):** 
```bash
make run2
```

**Exercise 1.3 (Buffon's Experiment):**
```bash
make run3
```

Alternatively, to run all simulations in sequence:
```bash
make run
```

### Clean Up
To remove the hidden build directory and generated seed files:
```bash
make clean
```

## Data Analysis
The simulation results are processed in the `analysis1.ipynb` notebook.