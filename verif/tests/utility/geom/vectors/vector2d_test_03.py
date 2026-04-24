# -*- coding: utf-8 -*-
'''Check that product by numpy.float64.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import numpy as np

vec1= geom.Vector2d(1,0)
f64= np.float64(3.141592)
productA= vec1*f64 # This works.
productB= float(f64)*vec1 # This also works.
# productC= f64*vec1 # This DOES NOT work.

err= math.sqrt((productA[0]-float(f64))**2+(productB[0]-float(f64))**2)

'''
print('typeA: ', type(f64))
print('productA= ', productA, flush= True)
print('typeB: ', type(float(f64)))
print('productB= ', productB, flush= True)
# print('typeC: ', type(f64))
# print('productC= ', productB, flush= True)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


