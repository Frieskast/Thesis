import uproot
import numpy as np
import matplotlib.pyplot as plt
import re

# --- Parameters ---
SRIM_FILE = "SRIM/BACKSCAT_1000.txt"
ROOT_FILE = "build/output1000.root"
ROOT_HIST_NAME = "Elevation;1"  # <-- Use your Geant4 elevation histogram name here!
N_INCIDENT = 50000
NBINS = 40
BIN_RANGE = (0, 1)

# --- Parse incident energy from SRIM header ---
incident_energy = None
incident_energy_str = None
plot_title = "Exit/Incident Energy Fraction: SRIM vs Geant4"
with open(SRIM_FILE, "r") as f:
    header_lines = [next(f) for _ in range(11)]
    for line in header_lines:
        # Match e.g. H(500 keV) or H(1 MeV)
        m = re.search(r"TRIM Calc\.\s*=\s*H\(([\d.,]+)\s*(keV|MeV|eV)\)", line)
        if m:
            incident_energy_val = float(m.group(1).replace(",", "."))
            incident_energy_unit = m.group(2)
            incident_energy_str = f"{incident_energy_val:g} {incident_energy_unit}"
            # Convert to eV for calculations
            if incident_energy_unit == "MeV":
                incident_energy = incident_energy_val * 1e6
            elif incident_energy_unit == "keV":
                incident_energy = incident_energy_val * 1e3
            else:
                incident_energy = incident_energy_val
            plot_title = f"XMM - SRIM vs Geant4 1 degree incident, {incident_energy_str} Protons"
            break
if incident_energy is None:
    incident_energy = 5e5  # fallback
    incident_energy_str = "500 keV"
    print("Warning: Could not parse incident energy from SRIM file, using default 500 keV.")

# --- SRIM Parsing for energy ratios ---
energy_ratios = []
backscattered_count = 0
srim_elevations = []
with open(SRIM_FILE, "r") as f:
    for line in f:
        line = line.strip()
        if not line or not line.startswith("B"):
            continue
        line = line.replace(",", ".")
        line = re.sub(r"^B(\d+)", r"B \1", line)
        parts = line.split()
        try:
            energy = float(parts[3])
            ratio = energy / incident_energy
            energy_ratios.append(ratio)
            backscattered_count += 1

            # SRIM direction cosines: Cos(X), Cos(Y), Cos(Z) are parts[7], [8], [9]
            cos_x = float(parts[8])
            elevation_deg = np.degrees(np.arccos(cos_x)) - 90.0
            srim_elevations.append(elevation_deg)

        except Exception:
            continue

print(f"Backscattered protons in SRIM data: {backscattered_count} ({backscattered_count/N_INCIDENT:.2%})")

# Histogram SRIM data
srim_hist, bin_edges = np.histogram(energy_ratios, bins=NBINS, range=BIN_RANGE)
srim_hist = srim_hist / N_INCIDENT

# Histogram Geant4 data
with uproot.open(ROOT_FILE) as f:
    h_g4 = f["Energy;2"]
    g4_vals = h_g4.values()
    g4_edges = h_g4.axis().edges()
    g4_hist = g4_vals / N_INCIDENT

# --- Elevation angle histogram (0-10 deg) ---
ELEV_BINS = 40
ELEV_RANGE = (0, 10)

# SRIM elevation histogram
srim_elev_hist, elev_bin_edges = np.histogram(srim_elevations, bins=ELEV_BINS, range=ELEV_RANGE)
srim_elev_hist = srim_elev_hist / N_INCIDENT

# Geant4 elevation histogram
with uproot.open(ROOT_FILE) as f:
    h_g4_elev = f[ROOT_HIST_NAME]
    g4_elev_vals = h_g4_elev.values()
    g4_elev_edges = h_g4_elev.axis().edges()
    # Restrict to 0-10 deg
    elev_mask = (g4_elev_edges[:-1] >= 0) & (g4_elev_edges[1:] <= 10)
    g4_elev_hist = g4_elev_vals[elev_mask] / N_INCIDENT
    g4_elev_bin_centers = 0.5 * (g4_elev_edges[:-1][elev_mask] + g4_elev_edges[1:][elev_mask])

# Print Geant4 backscattered count and fraction
g4_backscattered_count = np.sum(g4_elev_vals[elev_mask])
print(f"Backscattered protons in Geant4 data: {int(g4_backscattered_count)} ({g4_backscattered_count/N_INCIDENT:.2%})")

# SRIM bin centers
srim_elev_bin_centers = 0.5 * (elev_bin_edges[:-1] + elev_bin_edges[1:])
width = (elev_bin_edges[1] - elev_bin_edges[0]) * 0.4

# --- Calculate mean difference between SRIM and Geant4 exit angle distributions ---
# Interpolate Geant4 histogram to SRIM bin centers
g4_interp = np.interp(srim_elev_bin_centers, g4_elev_bin_centers, g4_elev_hist)
mean_diff = np.mean(np.abs(srim_elev_hist - g4_interp))
mean_diff_percent = mean_diff * 100
print(f"Mean absolute difference between SRIM and Geant4 exit angle distributions: {mean_diff:.3e} ({mean_diff_percent:.2f}%)")

# --- Calculate mean difference between SRIM and Geant4 exit angle distributions (0-10 deg only) ---
# Find indices for 0-10 deg in SRIM bin centers
angle_mask = (srim_elev_bin_centers >= 0) & (srim_elev_bin_centers <= 10)
mean_diff = np.mean(np.abs(srim_elev_hist[angle_mask] - g4_interp[angle_mask]))
mean_diff_percent = mean_diff * 100

print(f"Mean absolute difference (0-10 deg) between SRIM and Geant4 exit angle distributions: {mean_diff:.3e} ({mean_diff_percent:.2f}%)")

incident_angle_str = "1°"  # Or set dynamically if needed

# --- Plotting: Elevation angle (as line plot with points) ---
plt.figure()
plt.plot(
    srim_elev_bin_centers, srim_elev_hist, 
    marker='o', linestyle='-', color="#E69F00", label="SRIM"
)
plt.plot(
    g4_elev_bin_centers, g4_elev_hist, 
    marker='s', linestyle='-', color="#009E73", label="Geant4"
)
plt.xlabel("Exit Elevation Angle (deg)")
plt.ylabel("Fraction of Incident Protons")
plt.title(
    f"XMM - SRIM vs Geant4\n"
    f"Incident angle: {incident_angle_str}, Energy: {incident_energy_str}\n"
    f"Mean abs. diff (0-10°): {mean_diff_percent:.2f}%"
)
plt.xlim(0, 10)
plt.legend()
plt.grid(True)
plt.tight_layout()

# Format energy and angle for filename (replace spaces and special chars)
energy_str_file = incident_energy_str.replace(" ", "").replace(".", "p")
angle_str_file = incident_angle_str.replace("°", "deg")

# Save elevation angle plot
plt.savefig(
    f"/home/frisoe/geant4/geant4-v11.3.1/examples/projects/thesis/Plots/elevation_angle_{angle_str_file}_{energy_str_file}.png",
    dpi=1000
)
plt.show()

# --- (existing plot for energy ratios) ---
bin_centers = 0.5 * (bin_edges[:-1] + bin_edges[1:])
width = (bin_edges[1] - bin_edges[0]) * 0.4

# --- Calculate mean absolute difference for energy ratio histograms ---
energy_mask = (bin_centers >= 0) & (bin_centers <= 1)
mean_energy_diff = np.mean(np.abs(srim_hist[energy_mask] - g4_hist[energy_mask]))
mean_energy_diff_percent = mean_energy_diff * 100

# --- (existing plot for energy ratios) ---
plt.bar(bin_centers - width/2, srim_hist, width=width, label="SRIM", color="#E69F00", align="center", alpha=0.85, edgecolor="black")
plt.bar(bin_centers + width/2, g4_hist, width=width, label="Geant4", color="#009E73", align="center", alpha=0.7, edgecolor="black")

plt.xlabel("Exit/Incident Energy")
plt.ylabel("Fraction of Incident Protons")
plt.title(
    f"Exit/Incident Energy Fraction: SRIM vs Geant4\n"
    f"Incident angle: {incident_angle_str}, Energy: {incident_energy_str}\n"
    f"Mean abs. diff: {mean_energy_diff_percent:.2f}%"
)
plt.xlim(0,1)
plt.legend()
plt.grid(True)
plt.tight_layout()

# Save energy ratio plot
plt.savefig(
    f"/home/frisoe/geant4/geant4-v11.3.1/examples/projects/thesis/Plots/energy_ratio_{angle_str_file}_{energy_str_file}.png",
    dpi=1000
)
plt.show()

