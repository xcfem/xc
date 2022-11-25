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
from geotechnics import settlement

# Define loaded area
concentratedLoad= settlement.ConcentratedLoad(p= geom.Pos3d(0.5,0.5,0))


# Compute stress increment.
testPoints= [geom.Pos3d(0.25,0.25,-1), geom.Pos3d(.25,0.75,-1), geom.Pos3d(0.75,0.25,-1), geom.Pos3d(0.75,0.75,-1)]
stresses= concentratedLoad.getStressIncrement(P= 1e3, points= testPoints)

# Check results
refValues= 4*[355.6805199633012]
err= 0.0
for v, rv in zip(stresses, refValues):
    err+= (v-rv)**2
err+= math.sqrt(err)

'''
print(stresses)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
