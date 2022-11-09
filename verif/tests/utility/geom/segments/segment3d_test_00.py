# -*- coding: utf-8 -*-
''' Segment3d trivial test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

# Define a line.
fromPoint=geom.Pos3d(0,0,0)
toPoint=geom.Pos3d(10,0,0)
s1=geom.Segment3d(fromPoint,toPoint)

# get segment midpoint.
midPoint= s1.getMidPoint()
refDist= 10.0/2.0
ratio0= abs(midPoint.dist(fromPoint)-refDist)/refDist

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


