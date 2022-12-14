# -*- coding: utf-8 -*-
''' Verification of the getBufferPolygon method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

# Create test polyline.
pol1=geom.Polyline2d([geom.Pos2d(0,0), geom.Pos2d(1,0), geom.Pos2d(1,1)])

# Compute buffer polygon.
contour= pol1.getBufferPolygon(0.25, 8)

# Check result.
refContour= [geom.Pos2d(-0.25, 0), geom.Pos2d(-0.17677, -0.17677), geom.Pos2d(0, -0.24999), geom.Pos2d(0, -0.25), geom.Pos2d(1.25, -0.25), geom.Pos2d(1.25, 1), geom.Pos2d(1.17677, 1.17677), geom.Pos2d(1, 1.25), geom.Pos2d(0.82322, 1.17677), geom.Pos2d(0.75, 1), geom.Pos2d(0.75, 0.25), geom.Pos2d(0, 0.25), geom.Pos2d(-0.17677, 0.17677)]

err= 0
for p, refP in zip(contour.getVertexList(), refContour):
    err+= p.dist2(refP)
err= math.sqrt(err)

#print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err<1e-6:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


