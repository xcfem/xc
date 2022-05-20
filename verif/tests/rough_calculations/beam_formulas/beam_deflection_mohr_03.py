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

# Bending moment along the beam.
Mmax= prestressingLoad*eccentricity
a= 0.25
xi= [0, a*beam.l, (1-a)*beam.l, beam.l]
yi= [0.0, -Mmax, -Mmax, 0.0]
interpM= interpolate.interp1d(xi, yi, fill_value='extrapolate', bounds_error=False)

def M(x):
    '''Bending moment law.'''
    return interpM(x)

fMidSpan= beam.computeDeflection(x= beam.l/2.0, M= M)
PL2eP_EI= prestressingLoad*eccentricity*beam.l**2/beam.EI()
fMidSpanRef= (3-4*a**2)*PL2eP_EI/24.0 # See https://estructurando.net/2017/06/26/contraflecha-debida-al-pretensado/
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
# plt.show()
