# -*- coding: utf-8 -*-
''' Compute the response spectrum of a given ground motion.'''

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato"
__copyright__= "Copyright 2026, AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es"

import xc
import math
import numpy as np

def compute_response_spectrum(accelerations, dtA, dt= None, zLst= [.05], T_min= 1e-5, T_max= 5, mass= 1.0, Fy= 1e16, alpha= .01, uresidual= 0.0, max_prev_displ= 0.0, silent= True):
    ''' Compute the response spectrum of the given ground motion.

    :param accelerations: ground motion accelration values.
    :param dtA: time step for input data.
    :param dt: time step for analysis (if None make it equal to dtA).
    :param zLst: a list of damping ratios to compute the response with.
    :param T_min: lowest period to consider.
    :param T_max: largest period to consider.
    :param mass: mass of the SDOF oscillator.
    :param Fy: yielding strength (defaults to a big value so the material does
               not yield).
    :param alpha: strain-hardening ratio (defaults to .01).
    :param uresidual: residual displacement at the end of previous analysis
                      (optional, default=0).
    :param max_prev_displ: previous displacement (optional, default=0).
    :param silent: if true don't issue any message.
    '''
    # Define a period range below
    if(dt is None):
        dT= dtA # time step for analysis.

    # SDOF response object.
    m= 1.0 # mass.
    zeta= zLst[0] # damping ratio (to be set later).
    k= 0.0 # stiffness (to be set later).
    if(m!=1.0):
        forces= [m*a for a in accelerations]
    else:
        forces= accelerations
    sdfResponse= xc.SDOFResponse(m, zeta, k, Fy, alpha)

    # Use nested loops to analyse the system for various damping ratios and periods.
    data_frame = dict()
    for z in zLst:    
        resp = {'T':[0.0],'SD':[0.0], 'SV':[0.0], 'SA':[0.0], 'TA':[0.0], 'STA':[0.0], 'TTA':[0.0]}
        for T in np.arange(T_min, T_max, dT):

            k = (2*math.pi/T)**2 # mass = 1.0
            sdfResponse.setStiffness(k)
            sdfResponse.setDampingRatio(z)
            sr= sdfResponse.getResponse(dtA, dT, forces, uresidual, max_prev_displ)
            resp['SD'].append(sr['max_displ'])
            resp['SV'].append(sr['max_vel'])
            resp['SA'].append(sr['max_accel'])
            resp['TA'].append(sr['time_max_accel'])
            resp['STA'].append(sr['max_true_accel'])
            resp['TTA'].append(sr['time_max_true_accel'])
            resp['T'].append(T)
        # Appending keys and values dynamically
        data_frame[z] = resp
        if(not silent):
            print('Done with zeta= '+str(z)+'!')
    return data_frame
