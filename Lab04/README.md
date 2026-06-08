# Laboratory 04 - Molecular Dynamics and Ergodicity

This directory contains the C++ implementation and data analysis for the fourth laboratory session. The project focuses on simulating a Lennard-Jones gas using Molecular Dynamics (MD) via the Verlet integration algorithm. It explores the ergodic hypothesis by sampling the Maxwell-Boltzmann velocity distribution, studies the thermalization process starting from a highly ordered low-entropy state, and tests the limits of time reversibility due to macroscopic Lyapunov instability.

## Project Structure
- `SOURCE/system.cpp` & `SOURCE/system.h`: Core C++ implementation of the simulation engine. It includes the Verlet integration scheme, force calculations with Periodic Boundary Conditions (PBC), and data blocking measurements.
- `SOURCE/main.cpp`: Main execution file that initializes the system, runs the blocks/steps loop, and finalizes the simulation.
- `INPUT/`: Directory containing the simulation settings:
  - `input.dat`: Main parameters (temperature, density, cut-off radius, blocks, steps, restart flag).
  - `properties.dat`: Flags to enable/disable specific property measurements (e.g., potential energy, temperature, POFV).
  - `CONFIG/`: Contains the starting spatial configurations (e.g., the ideal `config.fcc` lattice).
- `OUTPUT/`: Target directory for the simulation results (e.g., `temperature.dat`, `pofv.dat`) and the updated phase-space configurations.
- `analysis4.ipynb` & `analysis4_2.ipynb`: Jupyter Notebooks containing the data analysis, $\chi^2$ statistical testing, theoretical comparisons, and visualization of the thermalization and reversibility experiments.
- `Makefile`: Automated build system for compilation and execution.

## Compilation and Usage
The project uses a hidden `.build/` directory to store object files and executables, keeping the root directory clean.

### Build
To compile the simulator:

```bash
make
```

### Execution

The simulation is driven by the configuration files in the `INPUT/` directory. To run the different parts of the exercise, you must edit `INPUT/input.dat` and `INPUT/properties.dat` accordingly, then execute the simulator:

```bash
time ./simulator.exe
```

### Simulation set up

For each exercise I made sure to put the set up instruction in `analysis4.ipynb` and in comments inside the code. Searching for the "COMMENT" and "UNCOMMENT" lines in `system.cpp` one should be able to reconstruct every step to setup the simulations.

### Clean Up
To remove the hidden build directory and generated seed files:
```bash
make clean
```

### Data Analysis

The simulation results are processed in the provided Jupyter notebook `analysis4.ipynb`. The analysis includes:
1. **Speed Distribution:** Comparing the progressive Monte Carlo velocity histograms (`pofv.dat`) against the theoretical Maxwell-Boltzmann distribution, including a detailed $\chi^2$ test and analysis of finite-size effects.
2. **Thermalization:** Visualizing the transient phase of the temperature and velocity distribution as the system relaxes from a highly improbable, low-entropy initial state.
3. **Lyapunov Instability:** Plotting the divergence of microscopic trajectories after a time-inversion event, demonstrating how floating-point round-off errors break the theoretical reversibility of the Verlet algorithm.
