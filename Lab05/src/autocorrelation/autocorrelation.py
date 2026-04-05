import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# 1. Carica i dati generati dal C++
print("Caricamento dati...")
r_values = np.loadtxt("raw_radius_1s.dat")

# 2. Funzione per calcolare l'autocorrelazione in modo efficiente
def compute_autocorrelation(x, max_lag):
    n = len(x)
    mean = np.mean(x)
    var = np.var(x)
    
    # Sottraiamo la media
    x_centered = x - mean
    
    acf = np.zeros(max_lag)
    for tau in range(max_lag):
        # Somma di x(t) * x(t+tau)
        sum_prod = np.sum(x_centered[:n-tau] * x_centered[tau:])
        # Normalizziamo dividendo per n e per la varianza
        acf[tau] = (sum_prod / (n - tau)) / var
        
        if tau % 50 == 0:
            print(f"Calcolo tau = {tau}/{max_lag}", end='\r')
            
    print("\nCalcolo completato.")
    return acf

# Decidiamo fino a che "ritardo" (tau) calcolare. 
# Di solito i primi 300-500 passi sono sufficienti per vedere il decadimento a zero.
max_tau = 300 
lags = np.arange(max_tau)
acf = compute_autocorrelation(r_values, max_tau)

# 3. Fit esponenziale per trovare il tempo di correlazione (tau_c)
# L'autocorrelazione decade circa come exp(-tau / tau_c)
def exponential_decay(tau, tau_c):
    return np.exp(-tau / tau_c)

# Fittiamo i dati (escludiamo tau=0 perché vale per forza 1)
popt, pcov = curve_fit(exponential_decay, lags[1:], acf[1:])
tau_c = popt[0]

print(f"\n=======================================================")
print(f" IL TEMPO DI CORRELAZIONE STIMATO E': tau_c = {tau_c:.1f} step")
print(f"=======================================================\n")

# 4. Plot dei risultati
plt.figure(figsize=(10, 6))
plt.plot(lags, acf, label="Autocorrelazione misurata", marker='.', linestyle='none')
plt.plot(lags, exponential_decay(lags, tau_c), label=f"Fit exp: $\\tau_c$ = {tau_c:.1f}", color='red')
plt.axhline(0, color='black', linestyle='--')
plt.xlabel("Distanza temporale (Lag, $\\tau$)")
plt.ylabel("Autocorrelazione $Ac(\\tau)$")
plt.title("Studio dell'Autocorrelazione per l'atomo di Idrogeno (1s)")
plt.legend()
plt.grid(True)
plt.show()