import numpy as np
from scipy.optimize import curve_fit
import subprocess
from matplotlib import pyplot as plt
import sys

# =====================================================
# UTILITIES
# =====================================================

# Compute autocorrelation function
def compute_autocorrelation(x, max_lag):
    n = len(x)
    mean = np.mean(x)
    var = np.var(x)
    
    x_centered = x - mean
    
    acf = np.zeros(max_lag)
    for tau in range(max_lag):
        # Sum of x(t) * x(t+tau)
        # Equivalent of c++
            # double sum = 0;
            #for(int t = 0; t < n - tau; t++) {
            #    sum += x[t] * x[t + tau];
            #}
        sum_prod = np.sum(x_centered[:n-tau] * x_centered[tau:])
        
        # Divide for the number of summed terms (number of pairs at distance tau: n-tau) and for the variance
        acf[tau] = (sum_prod / (n - tau)) / var
        
    return acf


# Function for exponential decay fit
def exponential_decay(tau, tau_c):
    return np.exp(-tau / tau_c)


# Calculate burn-in using moving average
def burn_in_calculation(r_array, expected_value, tolerance, window=50):
    """
    Calculates burn-in using moving average to avoid noise issues.
    We look for the first point where the moving average of r_array enters the range [expected_value - tolerance, expected_value + tolerance].
     - r_array: array of r values
     - expected_value: the expected value of r at equilibrium (e.g. 1.5 for 1s, 5.0 for 2p)
     - tolerance: how close we want to be to the expected value (e.g. 0.1)
     - window: the size of the moving average window (e.g. 50 steps)
     - returns: the index of the first step where the system is considered equilibrated (burn-in step)
    """
    # Moving average
    # np.convolve takes a window of length 'window' and slides it across the data, calculating the average of the values in that window.
    # The weights are uniform (1/window) to calculate the simple moving average.
    weights = np.ones(window) / window
    # Sums the products of the weights and the values in r_array for each position of the window, giving us the moving average.
    moving_average = np.convolve(r_array, weights, mode='valid')
    
    # Where does the moving average enter the range [expected_value - tolerance, expected_value + tolerance]?
    stable_indexes = np.argwhere(np.abs(moving_average - expected_value) < tolerance)
    
    if len(stable_indexes) > 0:
        # First stable index of the moving average
        # Adding window slides to the last element of the first window which moving average has been found stable.
        # This way we are sure that the system has actually thermalized (it thermalized "during" the window)
        burn_in = stable_indexes[0][0] + window
        return burn_in
    else:
        print("WARNING: The system has never stabilized!")
        return len(r_array)


print("Starting pipeline for generation and analysis of burn-in and autocorrelation function")
# Redirect the output to file
with open('../../outputs/autocorr/out.txt', 'w') as f:
    
    orig_stdout = sys.stdout
    sys.stdout = f

    print("\n\n====================================================")
    print(" AUTOMATIC PIPELINE: AUTOCORRELATION AND BURN-IN")
    print("====================================================\n")

    # ====================================================================
    # Raw data generation (autocorr_generator.cpp)
    # ====================================================================
    print("Running 'autocorr_generator.cpp'...")
    subprocess.run(["./.build/autocorr_generator.exe"], cwd="../../", stdout=f, text=True)
    print("...Autocorrelation data generation completed!\n")


    # ====================================================================
    # Analysis for burnin and autocorrelation time
    # ====================================================================
    print("Analysis for burnin and autocorrelation time")

    # Data loading
    path = "../../outputs/"
    r_1s_U = np.loadtxt(path + "autocorr/autocorr_r100_U.dat")
    r_2p_U = np.loadtxt(path + "autocorr/autocorr_r210_U.dat")
    r_1s_G = np.loadtxt(path + "autocorr/autocorr_r100_G.dat")
    r_2p_G = np.loadtxt(path + "autocorr/autocorr_r210_G.dat")

    # Burn-in
    expected_r_1s = 1.5
    expected_r_2p = 5.0
    tolerance = 0.1

    burn_in_idx_1s_U = burn_in_calculation(r_1s_U, expected_r_1s, tolerance)
    burn_in_idx_2p_U = burn_in_calculation(r_2p_U, expected_r_2p, tolerance)
    burn_in_idx_1s_G = burn_in_calculation(r_1s_G, expected_r_1s, tolerance)
    burn_in_idx_2p_G = burn_in_calculation(r_2p_G, expected_r_2p, tolerance)

    print(f" -> Thermalization found at index:")
    print(f"        - 1s U: {burn_in_idx_1s_U} steps")
    print(f"        - 2p U: {burn_in_idx_2p_U} steps")
    print(f"        - 1s G: {burn_in_idx_1s_G} steps")
    print(f"        - 2p G: {burn_in_idx_2p_G} steps\n")

    # Find worst case scenario for burn-in (the one that takes more steps to thermalize)
    burn_in_idx = max(burn_in_idx_1s_U, burn_in_idx_2p_U, burn_in_idx_1s_G, burn_in_idx_2p_G)
    print(f" -> Worst case burn-in: {burn_in_idx} steps\n")


    # ====================================================================
    # Plot Equilibration Phase (Burn-in)
    # ====================================================================
    print("Generating equilibration plots...")

    labels = ["1s state (Uniform)", "2p state (Uniform)", "1s state (Gaussian)", "2p state (Gaussian)"]

    # Calculate how many steps to plot: at least 1000 steps, or 4 times the burn-in phase (to show clearly the thermalization process)
    steps_to_plot = max(1000, int(burn_in_idx * 4))
    steps_arr = np.arange(steps_to_plot)

    fig_eq, axes_eq = plt.subplots(2, 2, figsize=(15, 10))
    axes_eq = axes_eq.flatten()

    raw_data_list = [r_1s_U, r_2p_U, r_1s_G, r_2p_G]
    expected_vals = [expected_r_1s, expected_r_2p, expected_r_1s, expected_r_2p]
    burn_in_vals = [burn_in_idx_1s_U, burn_in_idx_2p_U, burn_in_idx_1s_G, burn_in_idx_2p_G]

    for ax, data, label, exp_val, bi_val in zip(axes_eq, raw_data_list, labels, expected_vals, burn_in_vals):

        ax.plot(steps_arr, data[:steps_to_plot], marker='.', linestyle='-', color='blue', alpha=0.4, markersize=3, label='r trace')
        ax.axhline(exp_val, color='red', linestyle='--', linewidth=2, label=f"Expected $\\langle r \\rangle$ = {exp_val}")
        ax.axvline(bi_val, color='green', linestyle='-', linewidth=2, label=f"Detected Burn-in ({bi_val})")

        ax.set_title(f"Equilibration Phase: {label}", fontsize=13)
        ax.set_xlabel("Monte Carlo Step")
        ax.set_ylabel("Instantaneous radius $r$ ($a_0$)")
        ax.legend(loc="upper right")
        ax.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(path + "autocorr/equilibration_analysis.png", dpi=300)
    print("Equilibration plots saved!\n")



    #===================================================================
    # Autocorrelation Analysis
    #===================================================================

    # Cut the data to keep only the equilibrated part
    r_1s_U_equil = r_1s_U[burn_in_idx:]
    r_2p_U_equil = r_2p_U[burn_in_idx:]
    r_1s_G_equil = r_1s_G[burn_in_idx:]
    r_2p_G_equil = r_2p_G[burn_in_idx:]

    # Calculate autocorrelation on equilibrated data
    max_tau = 300
    lags = np.arange(max_tau)

    acf1s_U = compute_autocorrelation(r_1s_U_equil, max_tau)
    acf2p_U = compute_autocorrelation(r_2p_U_equil, max_tau)
    acf1s_G = compute_autocorrelation(r_1s_G_equil, max_tau)
    acf2p_G = compute_autocorrelation(r_2p_G_equil, max_tau)

    acfs = [acf1s_U, acf2p_U, acf1s_G, acf2p_G]

    # ====================================================================
    # Calculate Autocorrelation and Plot
    # ====================================================================

    labels = ["1s state (Uniform)", "2p state (Uniform)", "1s state (Gaussian)", "2p state (Gaussian)"]

    # Fit the data to the exponential curve and plot results
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    axes = axes.flatten()

    print("\nExponential Fit Results for Correlation Times:")
    print("=" * 60)

    worst_tau_c = 0.0

    for acf, label, ax in zip(acfs, labels, axes):

        # Perform the exponential fit (starting from index 1 to avoid tau=0 where Ac=1 perfectly)
        popt, pcov = curve_fit(exponential_decay, lags[1:], acf[1:])
        tau_c = popt[0]

        if tau_c > worst_tau_c:
            worst_tau_c = tau_c


        print(f"[{label}] -> Estimated tau_c: {tau_c:.1f} steps")

        # Plot the data and the fit
        ax.plot(lags, acf, label="Measured Data", marker='.', linestyle='none', markersize=4, color='blue')
        ax.plot(lags, exponential_decay(lags, tau_c), label=f"Exp. Fit: $\\tau_c$ = {tau_c:.1f}", color='red', linewidth=2)
        ax.axhline(0, color='black', linestyle='--', alpha=0.5)
        ax.axvline(tau_c, color='coral', linestyle='--', alpha=0.5, label=f"$\\tau_c$ = {tau_c:.1f} steps")

        ax.set_title(f"Autocorrelation: {label}", fontsize=13)
        ax.set_xlabel("Time Lag ($\\tau$)")
        ax.set_ylabel("Autocorrelation $Ac(\\tau)$")
        ax.legend(loc="upper right")
        ax.grid(True, alpha=0.3)

    print("=" * 60, "\n")

    plt.tight_layout()
    plt.savefig(path + "/autocorr/autocorrelation_analysis.png", dpi=300)

# Reset normal output
sys.stdout = orig_stdout

# ====================================================================
# PARAMETERS SELECTION AND EXPORT
# ====================================================================

# Equilibration: 10 times the worst burn_in phase
equilibration_steps = int(burn_in_idx * 10) 

# Correlation time (block length): 100 times the worst tau_c found among the 4 cases
M = 100                       # Number of blocks
L = int(worst_tau_c * 1000)    # Block length (100 times the worst tau_c)
N = L * M                     # Total number of steps (100 blocks)

print("Saving congiguration in 'config.txt'...")
with open("config.txt", "w") as f:
    f.write(f"EQUILIBRATION_STEPS {equilibration_steps}\n")
    f.write(f"BLOCK_LENGTH {L}\n")
    f.write(f"NUMBER_OF_BLOCKS {M}\n")
    f.write(f"TOTAL_STEPS {N}\n")

print("Configuration saved correctly!\n")

print("Pipeline terminated. Outputs in outputs/autocorr/out.txt.")