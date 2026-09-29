# -*- coding: utf-8 -*-
''' Computation of Rayleigh damping coefficients. 
Inspired on:

https://help.scia.net/25.0/en/analysis/modal_analysis_and_dynamics_and_seismicity/dynamics_basics/harmonic_damping_rayleigh.htm
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np
from solution import rayleigh_damping as rd

# --- USER INPUT VALUES ---
freq1= 1.0 # Fundamental frequency (Hz)
freq2= 10.0 # Dominant higher-mode frequency (Hz)
target_zeta= 0.03  # 3% target structural damping

# Compute coefficients
alpha, beta = rd.compute_rayleigh_damping(freq1, freq2, target_zeta)

# Reference values obtained from:
# https://help.scia.net/25.0/en/analysis/modal_analysis_and_dynamics_and_seismicity/dynamics_basics/harmonic_damping_rayleigh.htm

refAlpha= 0.342719198573432
ratio1= abs(alpha-refAlpha)/refAlpha
refBeta= 0.000868117871410338
ratio2= abs(beta-refBeta)/refBeta

'''
print(ratio1)
print(ratio2)
print('--- Rayleigh Damping Coefficients ---')
print(f'Alpha (Mass proportional):     {alpha:.6f}')
print(f'Beta  (Stiffness proportional): {beta:.6f}')
print('-------------------------------------')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-12 and abs(ratio2)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # --- VISUALIZATION ---
# import matplotlib.pyplot as plt
# # Generate a range of frequencies to plot the continuous damping curve
# f_range = np.linspace(0.1, freq2 * 1.5, 500)
# w_range = 2 * np.pi * f_range

# # Rayleigh damping ratio formula: zeta = (alpha / (2 * omega)) + (beta * omega / 2)
# zeta_mass = alpha / (2 * w_range)
# zeta_stiffness = (beta * w_range) / 2
# zeta_total = zeta_mass + zeta_stiffness

# # Plotting the results
# plt.figure(figsize=(10, 6))
# plt.plot(f_range, zeta_total * 100, 'b-', label='Total Rayleigh Damping', linewidth=2)
# plt.plot(f_range, zeta_mass * 100, 'r--', label='Mass Component ($\\alpha$)', alpha=0.7)
# plt.plot(f_range, zeta_stiffness * 100, 'g--', label='Stiffness Component ($\\beta$)', alpha=0.7)

# # Highlight target control points
# plt.scatter([freq1, freq2], [target_zeta * 100, target_zeta * 100], color='black', zorder=5, 
#             label=f'Target Modes ({target_zeta*100:.1f}%)')
# plt.text(freq1, target_zeta * 105, f' f1={freq1}Hz', verticalalignment='bottom')
# plt.text(freq2, target_zeta * 105, f' f2={freq2}Hz', verticalalignment='bottom')

# plt.title('Rayleigh Damping Ratio vs. Frequency', fontsize=14, fontweight='bold')
# plt.xlabel('Frequency (Hz)', fontsize=12)
# plt.ylabel('Damping Ratio (%)', fontsize=12)
# plt.ylim(0, max(target_zeta * 200, 15))  # Auto-scale Y-axis limits
# plt.grid(True, linestyle=':', alpha=0.6)
# plt.legend(loc='upper right', fontsize=11)
# plt.show()
