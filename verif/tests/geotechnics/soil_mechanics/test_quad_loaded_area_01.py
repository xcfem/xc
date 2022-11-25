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
from geotechnics import settlement

# Define loaded area
loadedArea= settlement.QuadLoadedArea(p1= geom.Pos3d(0,0,0),
                                      p2= geom.Pos3d(1,0,0),
                                      p3= geom.Pos3d(1,1,0),
                                      p4= geom.Pos3d(0,1,0))

# Compute sample points.
samplePoints= loadedArea.getSamplePoints(eSize= 0.5)

# Check results.
refPoints= [geom.Pos3d(0.25,0.25,0), geom.Pos3d(.25,0.75,0), geom.Pos3d(0.75,0.25,0), geom.Pos3d(0.75,0.75,0)]
err= 0.0
for p, pr in zip(samplePoints, refPoints):
    err+= p.dist2(pr)
err+= math.sqrt(err)

'''
print(samplePoints)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
