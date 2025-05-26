import uproot
import numpy as np
import matplotlib.pyplot as plt
import re

# --- Parameters ---
SRIM_FILE = "../SRIM/BACKSCAT_100.txt"
ROOT_FILE = "../build/output17.root"
ROOT_HIST_NAME = "ExitEnergyFraction"  # Change if your histogram has a different name
N_INCIDENT = 50000
INCIDENT_ENERGY = 1e5  # eV (100 keV)
NBINS = 40
BIN_RANGE = (0, 1)

# --- SRIM Parsing ---
energy_ratios = []
with open(SRIM_FILE, "r") as f:
    data_section = False
    for line in f:
        if re.match(r"^B\s", line):  # Data lines start with 'B'
            data_section = True
        if not data_section:
            continue
        if not line.startswith("B"):
            continue
        # Replace commas with dots for float conversion
        line = line.replace(",", ".")
        parts = line.split()
        if len(parts) < 6:
            continue
        try:
            energy = float(parts[3])
            ratio = energy / INCIDENT_ENERGY
            energy_ratios.append(ratio)
        except Exception:
            continue

# Histogram SRIM data
srim_hist, bin_edges = np.histogram(energy_ratios, bins=NBINS, range=BIN_RANGE)
srim_hist = srim_hist / N_INCIDENT  # Normalize

# --- Geant4 ROOT Histogram ---
with uproot.open(ROOT_FILE) as f:
    h_g4 = f[ROOT_HIST_NAME]
    g4_vals = h_g4.values()
    g4_edges = h_g4.axis().edges()
    g4_hist = g4_vals / np.sum(g4_vals)  # Normalize to 1 (or use N_INCIDENT if you prefer)

# --- Plotting ---
bin_centers = 0.5 * (bin_edges[:-1] + bin_edges[1:])
width = (bin_edges[1] - bin_edges[0]) * 0.4

plt.bar(bin_centers - width/2, srim_hist, width=width, label="SRIM", color="red", align="center")
plt.bar(bin_centers + width/2, g4_hist, width=width, label="Geant4", color="blue", align="center", alpha=0.7)

plt.xlabel("Exit/Incident Energy")
plt.ylabel("Fraction of Incident Protons")
plt.title("Exit/Incident Energy Fraction: SRIM vs Geant4")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()

