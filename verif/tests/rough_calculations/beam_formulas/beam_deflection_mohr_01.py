''' Simple beam deflection under constant moment
    using Mohr theorem.'''

import numpy as np
import scipy.integrate
from rough_calculations import ng_beam as b
from rough_calculations import ng_simple_beam as sb

beam= sb.SimpleBeam(E=210e9, I= 3.49e-6, l= 10)

prestressingLoad= 10e3
eccentricity= 0.35

def M(x):
    '''Bending moment law.'''
    return -prestressingLoad*eccentricity

fMidSpan= beam.computeZeta(x= beam.l/2.0, M= M)
fMidSpanRef= prestressingLoad*eccentricity*beam.l**2/8.0/beam.EI()
ratio1= abs(fMidSpan-fMidSpanRef)/fMidSpanRef
    

'''
print('fMidSpan= ', fMidSpan)
print('fMidSpanRef= ', fMidSpanRef)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-11:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Display results.
import matplotlib.pyplot as plt
x= beam.samplePoints(x0= 0.0, x1= beam.l)
y= beam.computeCurvature(x, M)
th= beam.computeThetaValues(x, M= M)
f= beam.computeZetaValues(x, M= M)
plt.plot(x, y, '-')
plt.plot(x, th, '-')
plt.plot(x, f, '-')
plt.show()