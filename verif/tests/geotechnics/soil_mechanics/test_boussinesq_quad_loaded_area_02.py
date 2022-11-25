# -*- coding: utf-8 -*-
''' Trivial test for QuadLoadedArea objects.'''

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
loadedArea= boussinesq.QuadLoadedArea(p1= geom.Pos3d(0,0,0),
                                      p2= geom.Pos3d(1,0,0),
                                      p3= geom.Pos3d(1,1,0),
                                      p4= geom.Pos3d(0,1,0))

# Compute stress increment in some test points.
testPoints= [geom.Pos3d(0.25,0.25,-1), geom.Pos3d(.25,0.75,-1), geom.Pos3d(0.75,0.25,-1), geom.Pos3d(0.75,0.75,-1)]
stresses= loadedArea.getStressIncrement(q= 1e3, points= testPoints, eSize= 0.5)

# Check results
refValues= 4*[299.341104612446]
err= 0.0
for v, rv in zip(stresses, refValues):
    err+= (v-rv)**2
err+= math.sqrt(err)

'''
print(stresses)
print(refValues)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
