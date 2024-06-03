# -*- coding: utf-8 -*-
''' Compute a modulated noise signal.

Code inspired by: https://portwooddigital.com/2024/04/21/material-testing-with-white-noise/
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import random
from scipy.stats import norm

def modulator(t, t1, t2, t3, t4):
    ''' Modulate the pulse magnitudes so the first few pulses don't cause 
        immediate large yield excursions.'''
    if t <= t1 or t >= t4:
        return 0.0
    elif t < t2: # Ramp up
        return (t-t1)/(t2-t1)
    elif t < t3: # Plateau
        return 1.0
    else:        # Ramp down
        return 1.0 - (t-t3)/(t4-t3)

def compute_noise(nPulse, maxValue, transitionLength= 0.1):
    ''' Compute the noise signal and return the corresponding values.

    :param nPulse: number of pulses.
    :param maxValue: maximum value of the signal.
    :param transitionLength: length of the transitions at both ends of the equence.
    '''
    randomSeed= random.randint(1e5,1e6-1)
    random.seed(randomSeed)

    pulses= list(range(nPulse))
    noise= [norm.ppf(random.random()) for i in pulses]
    t1 = 0
    t2 = transitionLength*nPulse
    t3 = (1-transitionLength)*nPulse
    t4 = nPulse
    modulatedNoise= [noise[i]*modulator(i,t1,t2,t3,t4) for i in pulses]
    # Scale the sequence so that the maximum modulated pulse is of maxValue magnitude.
    maxNoise= max(abs(max(modulatedNoise)),abs(min(modulatedNoise)))
    modulatedNoise= [maxValue*modulatedNoise[i]/maxNoise for i in pulses]


    return pulses, modulatedNoise

