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

p0= geom.Pos2d(0,0)
p1= geom.Pos2d(0,1.5)
p2= geom.Pos2d(1,1.5)

ln= geom.Line2d(p1, p2)

hp= geom.HalfPlane2d(ln, p0)

points= [geom.Pos2d(-2,2)]
for i in range(1,10):
    sign= (-1)**i
    pt= geom.Pos2d(-2+i, sign)
    points.append(pt)
points.append(geom.Pos2d(-2+i+1, 2))
pline= geom.Polyline2d(points)

clipped= hp.clip(pline, 0.0)[0]

pInt0= geom.Pos2d(-5.5/3.0, 1.5)
pInt1= geom.Pos2d(23.5/3.0, 1.5)
ref_points= [pInt0]+points[1:-1]+[pInt1]
ref_pline= geom.Polyline2d(ref_points)
err= 0.0
for p1, p2 in zip(clipped.getVertices(), ref_pline.getVertices()):
    err+= (p1.x-p2.x)**2+(p1.y-p2.y)**2
err= math.sqrt(err)

# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
