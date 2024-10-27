# -*- coding: utf-8 -*-
'''Test Polyline2d::getProjection method. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

# Define polyline.
points= [geom.Pos2d(0, 0), geom.Pos2d(1, 0), geom.Pos2d(1,1)]
pline= geom.Polyline2d(points)

# Project a point.
pt= geom.Pos2d(1.5, 0.5)
projection= pline.getProjection(pt)
# Compute the distance along the polyline up to the projected point.
lengthUpTo= pline.getLengthUpTo(projection)

err= math.sqrt(projection.dist2(geom.Pos2d(1.0, 0.5))+(lengthUpTo-1.5)**2)

# Check getLengths method.
lengths= pline.getLengths() # Compute lengths.
refLengths= [0.0, 1.0, 2.0] # Reference lengths. 
errLengths= 0.0 
for l, lr in zip(lengths, refLengths):
    errLengths+= (l-lr)**2
errLengths= math.sqrt(errLengths)

'''
print('projection: ', projection)
print('length upto projection: ', lengthUpTo)
print(err)
print(lengths)
print(refLengths)
print(errLengths)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6) and (errLengths<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
