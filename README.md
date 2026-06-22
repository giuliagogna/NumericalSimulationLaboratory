# Numerical Simulation Laboratory

This repository contains the code, data, and analysis for the exercises developed during the **Numerical Simulation Laboratory** course, part of the Master's Degree in Physics at the University of Milan (Prof. Davide E. Galli).

## Repository Structure

* **Laboratory Sessions (`LabXX/`):** Each laboratory session has its own dedicated directory. Inside, you will find a local `README.md`, the source code, and a Jupyter Notebook containing the theoretical explanations and data analysis.
* **Shared Resources (`utils/`):** This directory contains custom C++ libraries and headers that are utilized across multiple laboratory sessions.

## Compilation and Execution

Basic instructions for compilation and execution are outlined in the `README.md` of each specific laboratory folder. For more complex exercises, detailed explanations regarding the implementation, execution parameters, and physical interpretation are embedded directly within the Jupyter Notebooks.

To keep the repository clean and uncluttered, all C++ executables and object files are strictly routed to local `.build/` directories.

## Extended Analysis and Data Disclaimer

For several exercises, I opted to perform extended analyses beyond the base requirements to ensure robust statistical results. Notable examples include:
* A chi-square test comparing the simulated velocity distribution against the theoretical Maxwell-Boltzmann distribution (Lab 04).
* An in-depth study of data autocorrelation and block sizing (Lab 05).
* Extended simulation runs with significantly larger datasets to minimize statistical uncertainties.

**Disclaimer:** Due to these extended runs, some output files are quite large. Specific warnings are provided in the relevant directories where file size might be a factor.