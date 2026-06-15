# Laboratory 07 - Molecular Dynamics and Monte Carlo

This directory contains the C++ implementation and data analysis for the seventh laboratory session. The project focuses on simulating the liquid phase of Argon using Molecular Dynamics (MD) via the Verlet integration algorithm in the microcanonical ensemble (NVE), and using the Metropolis Monte Carlo (MC) algorithm in the canonical ensemble (NVT). It explores the autocorrelation function of the potential energy per particle and the evolution of the statistical error, evaluated via the Data Blocking technique, with respect to the block length. Finally, it calculates the Radial Distribution Function $g(r)$ and the thermodynamic properties of pressure and potential energy to explore the structural and thermodynamic equivalence of the ensembles.

## Project Structure
- `SOURCE/system.cpp` & `SOURCE/system.h`: Core C++ implementation of the simulation engine. It includes the Verlet integration scheme, the MCMC Metropolis move proposals, Periodic Boundary Conditions (PBC), tail corrections, and data blocking accumulators.
- `SOURCE/main.cpp`: Main execution file that initializes the system, runs the blocks/steps loop, and finalizes the simulation.
- `INPUT/`: Directory containing the simulation settings:
  - `input.dat`: Main parameters (simulation type flag, restart flag, temperature, number of particles, density, cut-off radius, algorithm step size, number of blocks, number of steps per block).
  - `properties.dat`: Flags to enable/disable specific property measurements (e.g., potential energy, temperature, pressure, and $g(r)$).
  - `CONFIG/`: Contains the starting spatial configurations (e.g., the ideal `config.fcc` lattice) and the restart files needed for MD (`config.xyz`, `conf-1.xyz`).
- `OUTPUT/`: Target directory for the simulation results (e.g., `temperature.dat`, `gofr.dat`) and the updated phase-space configurations.
- `analysis7.ipynb`: Jupyter Notebook containing the data analysis, theoretical comparisons, and visualization of the equilibration and data blocking results.
- `SOURCE/Makefile`: Automated build system for compilation and execution.

## Compilation and Usage
The project uses a hidden `.build/` directory to store object files and executables, keeping the root directory clean.

### Build
To compile the simulator navigate to the directory `SOURCE/` and execute:

```bash
make
```

### Execution

The simulation is driven by the configuration files in the `INPUT/` directory. To run the different parts of the exercise, you must edit `INPUT/input.dat` and `INPUT/properties.dat` accordingly, then execute the simulator in `SOURCE/` with:

```bash
time ./simulator.exe
```

### Clean Up
To remove the hidden build directory and generated seed files:
```bash
make clean
```

### Data Analysis

The simulation results are processed in the provided Jupyter notebook `analysis7.ipynb`. The potential energy per particle and the pressure are calculated taking into account the analytical tail corrections. The analysis includes:
1. **Equilibration Analysis:** Visualization of the transient phase in the dynamics of the system starting from a perfect FCC lattice. For the NVE ensemble (MD), this includes the empirical determination of the initial temperature ($T_{start} = 1.95$) required to reach the target equilibrium temperature ($T^\star \approx 1.1$) after the initial drop caused by the melting of the crystalline structure. Instantaneous thermodynamic observables were recorded to verify thermalization.
2. **Autocorrelation Analysis:** Calculation of the autocorrelation function to determine the ideal block length required to obtain independent measurements.
3. **Error Analysis:** Study of the evolution of the Data Blocking statistical uncertainty as a function of the block length $L$, demonstrating the initial underestimation for correlated data and the exact mathematical cancellation effect at the saturation plateau.
4. **Thermodynamic Properties:** Progressive Data Blocking averages and uncertainties for potential energy $U/N$ and pressure $P$. The NVE and NVT results are compared to investigate the theoretical equivalence of the ensembles and explicitly discuss minor finite-size and temperature mismatch effects.
5. **Radial Distribution Function $g(r)$:** Final estimation of the spatial structural distribution in both ensembles, including an explicit calculation of their difference $\Delta g(r)$ with propagated statistical errors.