# Laboratory 06 - 1D Ising Model and Markov Chain Monte Carlo

This directory contains the C++ implementation, Python automation pipeline, and data analysis for the sixth laboratory session. The project focuses on simulating the 1D Ising Model using Markov Chain Monte Carlo (MCMC) techniques, specifically comparing the Metropolis algorithm and the Gibbs sampler. It explores the equilibration process, the "trapping effect" at different temperatures, and evaluates thermodynamic properties using the Data Blocking method to compare them against exact analytical solutions.

## Project Structure
- `SOURCE/system.cpp` & `SOURCE/system.h`: Core C++ implementation of the simulation engine. It includes the MCMC move proposals (Metropolis and Gibbs), Boltzmann weight evaluations, Periodic Boundary Conditions (PBC) for spins, and data blocking accumulators.
- `SOURCE/main.cpp`: Main execution file that initializes the system, runs the blocks/steps loop, and finalizes the simulation.
- `INPUT/`: Directory containing the simulation settings:
  - `input.dat`: Main parameters (simulation type flag, restart flag, temperature, numbr of particles, density, cut-off radius, algorithm step, number of blocks, number of steps, restart flag).
  - `properties.dat`: Flags to enable/disable specific property measurements (internal energy, specific heat, susceptibility, magnetization).
  - `CONFIG/`: Contains the starting spin configurations (`config.spin`).
- `OUTPUT/`: Target directory for the simulation results and the updated phase-space configurations.
- `pipeline.py`: A custom Python script that orchestrates the Simulated Cooling protocol and automates the execution across the temperature range.
- `analysis6.ipynb`: Jupyter Notebook containing the data analysis, theoretical comparisons, and visualization of the equilibration and data blocking results.
- `SOURCE/Makefile`: Automated build system for compilation and execution.

## Compilation and Usage
The project uses a hidden `.build/` directory to store object files and executables, keeping the root directory clean.

### Build
To compile the simulator navigate to the directory `SOURCE/` and execute:

```bash
make
```

### Execution - Equilibration
The first results in the notebook `analysis6.ipynb` regarding the equilibration can be achieved running the simulation as described in the notebook itself.
Execute the simulator in `SOURCE/` with:

```bash
time ./simulator.exe
```

### Clean Up
To remove the hidden build directory and generated seed files:
```bash
make clean
```

### Pipeline Execution Instructions
Since MCMC algorithms require running the system across a range of temperatures and ensuring statistical independence, the simulations are orchestrated by the `pipeline.py` script. This script automates the Simulated Cooling protocol: it runs a "hollow" equilibration phase, executes the measurement phase, properly renames the output files, and progressively lowers the temperature from T = 2.0 down to T = 0.5.

The Python pipeline is designed to run in two distinct modes, depending on the thermodynamic properties being measured. You must set the correct simulation type (2 for Metropolis, 3 for Gibbs) in `input.dat` before launching.

#### The whole Mode (Zero External Field)
To measure internal energy, specific heat, and susceptibility, the system must be simulated with no external magnetic field. In input.dat, set the parameters to J = 1.0 and h = 0.0. In properties.dat, ensure that only `TOTAL_ENERGY`, `SPECIFIC_HEAT`, and `SUSCEPTIBILITY` are enabled. From the root directory, run:

```bash
python pipeline.py whole
```

#### The magnetization Mode (Non-Zero External Field)
Because the theoretical magnetization of the 1D Ising model is strictly zero without an external field, a small field must be applied to observe this property. Before launching the script, manually edit input.dat to set the external magnetic field to h = 0.02. In `properties.dat`, enable only the `MAGNETIZATION` property. Run:

```bash
python pipeline.py magnetization
```

### Data Management and Storage Optimization
To keep the project repository lightweight and optimize storage, the pipeline includes an automatic data-shrinking feature. For the majority of the simulated temperatures, the script reduces the final .dat files by retaining only the header and the final block average. The full, uncompressed history of the data blocking procedure (all 100 blocks) is preserved intact strictly for three key temperatures: T = 2.0, T = 1.0, and T = 0.5. This allows for detailed statistical analysis of the convergence without cluttering the repository.

### Data Analysis

The simulation results are processed in the provided Jupyter notebook `analysis6.ipynb`. The analysis includes:

1. **Equilibration Phase:** Visualizing the instantaneous internal energy and magnetization starting from a random configuration (infinite temperature) to evaluate the necessary Monte Carlo equilibration time before taking measurements. These plots highlight the correlation time at low temperature and the "trapping effect"

2. **Data Blocking Convergence:** Plotting the progressive block averages and their statistical uncertainties for T = 2.0, T = 1.0, and T = 0.5.

3. **Theoretical Comparison:** Directly plotting the final simulated values for internal energy, specific heat, susceptibility, and magnetization against the exact analytical curves derived from the 1D Ising Model partition function.
