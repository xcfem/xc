# -*- coding: utf-8 -*-
'''Trivial half-plane test: clip a zig-zag polyline with a half plane.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

p0= geom.Pos2d(0,2)
p1= geom.Pos2d(0,0.5)
p2= geom.Pos2d(1,0.5)

ln= geom.Line2d(p1, p2)

hp= geom.HalfPlane2d(ln, p0)

points= [geom.Pos2d(-2,2)]
for i in range(1,10):
    sign= (-1)**i
    pt= geom.Pos2d(-2+i, sign)
    points.append(pt)
points.append(geom.Pos2d(-2+i+1, 2))
pline= geom.Polyline2d(points)

clipped= hp.clip(pline, 0.0)

ref_plines= [geom.Polyline2d([points[0], geom.Pos2d(-1.5,0.5)])]
x= -0.25
for i in range(0, 4):
    pA= geom.Pos2d(x, 0.5)
    pB= geom.Pos2d(x+0.25, 1.0)
    pC= geom.Pos2d(x+0.5, 0.5)
    ref_plines.append(geom.Polyline2d([pA, pB, pC]))
    x+= 2.0

ref_plines.append(geom.Polyline2d([geom.Pos2d(7.5,0.5), points[-1]]))
err= 0.0

for pline, ref_pline in zip(clipped, ref_plines):
    for p1, p2 in zip(pline.getVertices(), ref_pline.getVertices()):
        err+= (p1.x-p2.x)**2+(p1.y-p2.y)**2

err= math.sqrt(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
