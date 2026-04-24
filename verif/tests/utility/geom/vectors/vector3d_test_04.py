# -*- coding: utf-8 -*-
'''Check the product by numpy.float64.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import numpy as np

vec1= geom.Vector3d(1,0,0)
f64= np.float64(3.141592)
productA= vec1*f64 # This works.
productB= float(f64)*vec1 # This also works.
productC= f64*vec1 # This also works.

err= (productA[0]-float(f64))**2+(productB[0]-float(f64))**2+(productC[0]-float(f64))**2

for v, vref in zip(vec1, [1,0]):
    err+= (v-vref)**2

err= math.sqrt(err)

'''
print('typeA: ', type(f64))
print('productA= ', productA, flush= True)
print('typeB: ', type(float(f64)))
print('productB= ', productB, flush= True)
print('typeC: ', type(f64))
print('productC= ', productB, flush= True)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
