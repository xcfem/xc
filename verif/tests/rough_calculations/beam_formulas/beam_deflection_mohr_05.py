# -*- coding: utf-8 -*-
''' Simple beam deflection under moment
    due to prestressing forces using Mohr theorem.'''

import numpy as np
import scipy.integrate
from scipy import interpolate
from rough_calculations import ng_beam as b
from rough_calculations import ng_simple_beam as sb
import geom
from geom_utils import parabola as pb

# Simple beam definition.
beam= sb.SimpleBeam(E=210e9, I= 3.49e-6, l= 10)

# Prestressing load.
prestressingLoad= 10e3
eccentricity= 0.35

# Bending moment along the beam due to prestressing.
Mmax= prestressingLoad*eccentricity
p0= geom.Pos2d(0,0.0)
p1= geom.Pos2d(beam.l/2.0,-Mmax)
p2= geom.Pos2d(beam.l,0.0)

parabola= pb.Parabola(p0,p1,p2)

def M(x):
    '''Bending moment law.'''
    return parabola.y(x)

fMidSpan= beam.computeDeflection(x= beam.l/2.0, M= M)
PL2eP_EI= prestressingLoad*eccentricity*beam.l**2/beam.EI()
fMidSpanRef= 5*PL2eP_EI/48.0
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
