# -*- coding: utf-8 -*-
'''Trivial half-plane test. Clip a polyline with a half-plane.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

p0= geom.Pos3d(0,0,2)
p1= geom.Pos3d(0,0,1)
p2= geom.Pos3d(1,0,1)
p3= geom.Pos3d(1,1,1)

plane= geom.Plane3d(p1, p2, p3)

hp= geom.HalfSpace3d(plane, p0)

points= [geom.Pos3d(-2,0,2), geom.Pos3d(0,0,0), geom.Pos3d(2,0,2)]
pline= geom.Polyline3d(points)

clipped= hp.clip(pline, 0.0)

ref_pline0= geom.Polyline3d([geom.Pos3d(-2,0,2), geom.Pos3d(-1,0,1)])
err= 0.0
for p1, p2 in zip(clipped[0].getVertices(), ref_pline0.getVertices()):
    err+= (p1.x-p2.x)**2+(p1.y-p2.y)**2+(p1.z-p2.z)**2
    
ref_pline1= geom.Polyline3d([geom.Pos3d(1,0,1), geom.Pos3d(2,0,2)])
for p1, p2 in zip(clipped[1].getVertices(), ref_pline1.getVertices()):
    err+= (p1.x-p2.x)**2+(p1.y-p2.y)**2+(p1.z-p2.z)**2
err= math.sqrt(err)

'''
print(clipped[0])
print(clipped[1])
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')