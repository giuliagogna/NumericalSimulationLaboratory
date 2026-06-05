Laboratory 03 - European Option Pricing via Monte Carlo
This directory contains the C++ implementation and data analysis for the third laboratory session, focusing on the estimation of European call and put option prices. It compares the analytical Black-Scholes solution with numerical Monte Carlo estimations using both direct sampling and discretized path simulations of Geometric Brownian Motion (GBM).

Project Structure
src/exercise03_1.cpp: Main source file for estimating option prices by directly sampling the final asset price S(T).

src/exercise03_2.cpp: Main source file for estimating option prices by sampling the discretized GBM path of the asset price.

../utils/random/: Specialized library for random number generation.

../utils/data_blocking/: Object-oriented implementation of the data blocking method for statistical uncertainty estimation.

../utils/pricing_options/: Shared functions containing the analytical Black-Scholes formulas.

../utils/auxiliary_functions/: Custom helper functions.

analysis3.ipynb: Jupyter Notebook containing the data analysis, theoretical comparisons, and visualization.

Makefile: Automated build system for compilation and execution.

Compilation and Usage
The project uses a hidden .build/ directory to store object files and executables, keeping the root directory clean.

Build
To compile all exercises at once:

Bash
make
Execution
You can run the exercises individually using the dedicated make targets:

Exercise 3.1 (Direct Sampling Method):

Bash
make run1
Exercise 3.2 (Discretized Path Sampling Method):

Bash
make run2
Alternatively, to run both simulations in sequence:

Bash
make run
Clean Up
To remove the hidden build directory and generated seed files:

Bash
make clean
Data Analysis
The simulation results are processed in the analysis3.ipynb notebook. The analysis compares the progressive Monte Carlo estimates and their statistical uncertainties (calculated via data blocking) against the expected Black-Scholes analytical prices.

Note: The raw data files are excluded from the repository to maintain a lightweight structure. The notebook contains the pre-rendered plots for immediate viewing on GitHub.