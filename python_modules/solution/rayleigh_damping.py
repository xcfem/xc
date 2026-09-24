# -*- coding: utf-8 -*-
''' Computation of Rayleigh damping coefficients. 
Inspired on:

https://help.scia.net/25.0/en/analysis/modal_analysis_and_dynamics_and_seismicity/dynamics_basics/harmonic_damping_rayleigh.htm
'''

import numpy as np

def compute_rayleigh_damping(f1, f2, zeta):
    '''
    Computes Rayleigh damping coefficients alpha (mass) and beta (stiffness).
    
    Parameters:
    f1 (float): First target natural frequency in Hz
    f2 (float): Second target natural frequency in Hz
    zeta (float): Target modal damping ratio (e.g., 0.05 for 5%)
    
    Returns:
    alpha, beta (float): Damping coefficients
    '''
    # Convert cyclic frequencies (Hz) to angular frequencies (rad/s)
    w1 = 2 * np.pi * f1
    w2 = 2 * np.pi * f2
    
    # Calculate alpha and beta assuming equal damping ratio for both modes
    denom = w1 + w2
    alpha = zeta * (2 * w1 * w2) / denom
    beta = zeta * 2 / denom
    
    return alpha, beta
