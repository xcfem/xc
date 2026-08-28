# -*- coding: utf-8 -*-
''' Check axial elongation under uniform axial load.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from scipy.constants import g
from materials.ec3 import EC3_materials # Steel shapes.
from rough_calculations import ng_cantilever as cv

steelType= EC3_materials.S235JR # steel type
E= steelType.E
A= 2e-3 # m2
b= math.sqrt(A)
I= 1/12*b**4
L= 20
beam= cv.Cantilever(E= E, A= A, I= I,l= L)

selfWeight= steelType.rho*A*g
delta= beam.getAxialDisplacementUnderUniformAxialLoad(w= selfWeight)
W= selfWeight*L
refDelta= W*L/2.0/E/A
ratio= abs(delta-refDelta)/refDelta

'''
print('W= ', W/1e3, 'kN')
print('delta= ', delta)
print('refDelta= ', refDelta)
print("ratio= ", ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

