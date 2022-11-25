# -*- coding: utf-8 -*-
''' Trivial test for ConcentratedLoad objects.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import math
from geotechnics import boussinesq

# Define loaded area
concentratedLoad= boussinesq.ConcentratedLoad(p= geom.Pos3d(0.5,0.5,0))


# Compute stress increment.
testPoints= [geom.Pos3d(0.25,0.25,-1), geom.Pos3d(.25,0.75,-1), geom.Pos3d(0.75,0.25,-1), geom.Pos3d(0.75,0.75,-1)]
vertStresses= concentratedLoad.getVerticalStressIncrement(P= -1e3, points= testPoints)
stressVectors= concentratedLoad.getStressIncrement(P= -1e3, points= testPoints)

# Check results
## Vertical stresses.
vStress= -355.6805199633012
refVertStresses= 4*[vStress]
err= 0.0
for v, rv in zip(vertStresses, refVertStresses):
    err+= (v-rv)**2

## Stress vector.
hStress= -28.357724863608592*math.sqrt(2)/2
refStressVectors= 4*[geom.Vector3d(hStress,hStress,vStress)]
for v, rv in zip(stressVectors, refStressVectors):
    err+= (v-rv).getModulus()**2
err+= math.sqrt(err)
    

'''
print(vertStresses)
print(refVertStresses)
print(stressVectors)
print(refStressVectors)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
