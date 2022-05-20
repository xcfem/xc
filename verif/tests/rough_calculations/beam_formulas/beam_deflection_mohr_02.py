# -*- coding: utf-8 -*-
''' Simple beam deflection under moment
    due to prestressing forces using Mohr theorem.'''

import numpy as np
import scipy.integrate
from scipy import interpolate
from rough_calculations import ng_beam as b
from rough_calculations import ng_simple_beam as sb

# Simple beam definition.
beam= sb.SimpleBeam(E=210e9, I= 3.49e-6, l= 10)

# Prestressing load.
prestressingLoad= 10e3
eccentricity= 0.35

# Bending moment along the beam due to prestressing.
Mmax= prestressingLoad*eccentricity
xi= [0, beam.l/2.0, beam.l]
yi= [0.0, -Mmax, 0.0]
interpM= interpolate.interp1d(xi, yi, fill_value='extrapolate', bounds_error=False)

def M(x):
    '''Bending moment law.'''
    return interpM(x)

fMidSpan= beam.computeDeflection(x= beam.l/2.0, M= M)
PL2eP_EI= prestressingLoad*eccentricity*beam.l**2/beam.EI()
fMidSpanRef= PL2eP_EI/12.0
ratio1= abs(fMidSpan-fMidSpanRef)/fMidSpanRef
    
'''
print('fMidSpan= ', fMidSpan)
print('fMidSpanRef= ', fMidSpanRef)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-3:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Display results.
# import matplotlib.pyplot as plt
# x= beam.samplePoints(x0= 0.0, x1= beam.l)
# y= beam.computeCurvature(x, M)
# f= beam.computeDeflectionValues(x, M= M)
# plt.plot(x, y, '-', x, f, '-')
# plt.xticks(np.arange(min(x), max(x)+1, 1))
# plt.grid()
# plt.show()
