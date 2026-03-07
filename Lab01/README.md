# Laboratory 01 - Random Number Techniques and Buffon's Experiment

This directory contains the C++ implementation and data analysis for the first laboratory session, focusing on the Central Limit Theorem, probability distributions, and the Monte Carlo estimation of $\pi$.

## Project Structure

- `exercise01-1.cpp`, `exercise01-2.cpp`, `exercise01-3.cpp`: main source files for the three exercises.
- `random/`: specialized library for random number generation (provided).
- `auxiliary_functions/`: custom helper functions.
- `analysis.ipynb`: Jupyter Notebook containing the data analysis and visualization.
- `Makefile`: automated build system for compilation and execution.

## Compilation and Usage

The project uses a hidden `.build/` directory to store object files and executables.

### Build
To compile all exercises at once:
make

### Execution
You can run the exercises individually using the dedicated make targets:
Exercise 1.1 (Mean and Variance): make run1
Exercise 1.2 (Central Limit Theorem): make run2
Exercise 1.3 (Buffon's Experiment): make run3

Alternatively, to run all simulations in sequence:
make run

### Data Analysis
The simulation results are processed in the analysis1.ipynb notebook.

Note: The raw data files (stored outputs/) are excluded from the repository to maintain a lightweight structure. The notebook contains the pre-rendered plots for immediate viewing on GitHub.