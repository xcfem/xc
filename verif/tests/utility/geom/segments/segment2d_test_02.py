# -*- coding: utf-8 -*-
''' Segment2d trivial test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

# Define a line.
fromPoint=geom.Pos2d(0,0)
toPoint=geom.Pos2d(10,0)
s1=geom.Segment2d(fromPoint,toPoint)

# get segment midpoint.
midPoint= s1.getMidPoint()
refDist= 10.0/2.0
ratio0= abs(midPoint.dist(fromPoint)-refDist)/refDist

# get a line parallel to the previous one, at a fixed distance
s2=s1.offset(2)
# the same, at the opposite side.
s3=s1.offset(-2)

# Compute the centroids.
p1=s2.getCenterOfMass()
p2=s3.getCenterOfMass()

# Verify that the distance results as expected.
ratio1=math.fabs(p1.dist(p2)-4)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0<1e-15) and (ratio1<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


