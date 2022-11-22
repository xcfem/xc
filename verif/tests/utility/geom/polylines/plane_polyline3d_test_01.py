# -*- coding: utf-8 -*-
''' Test getIVectorAtLength, getJVectorAtLength methods.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

# Points.
#
#    p5              p6
#    +--------------+
#    |
#    |
#    |
#    |
#    |
#    |
#    +--------------+ p3
#   p4              |
#                   |
#                   |
#                   |
#                   |
#                   |
#    +--------------+ p2
#    p1

# Vertex list.
vertices= [geom.Pos3d(0, 0, 0), geom.Pos3d(1, 0, 0), geom.Pos3d(1,1, 0), geom.Pos3d(0, 1, 0), geom.Pos3d(0,2, 0), geom.Pos3d(1,2, 0)]

# Define polyline.
plane_pline_3d= geom.PlanePolyline3d(vertices)

# Check results at different points.
lengths= [0.5, 1.5, 2.5, 3.5, 4.5]
# Reference values.
vIRef= [geom.Vector3d(1.0,0.0, 0), geom.Vector3d(0.0,1.0, 0), geom.Vector3d(-1.0,0.0, 0), geom.Vector3d(0.0,1.0, 0), geom.Vector3d(1.0,0.0, 0)]
vJRef= [geom.Vector3d(0.0,1.0, 0), geom.Vector3d(-1.0,0.0, 0), geom.Vector3d(0.0,-1.0, 0), geom.Vector3d(1.0,0.0, 0), geom.Vector3d(0.0,-1.0, 0)]
pointsRef= [geom.Pos3d(0.5,0.0, 0), geom.Pos3d(1.0,0.5, 0), geom.Pos3d(0.5,1.0, 0), geom.Pos3d(0.0,1.5, 0), geom.Pos3d(0.5, 2.0, 0)]
err= 0.0

for l, vIr in zip(lengths, vIRef):
    vI= plane_pline_3d.getIVectorAtLength(l) # I vector at s= l
    err+= (vI-vIr).getModulus()**2
for l, vJr in zip(lengths, vJRef):
    vJ= plane_pline_3d.getJVectorAtLength(l) # J vector at s= l
    err+= (vJ-vJr).getModulus()**2
for l, pr in zip(lengths, pointsRef):
    p= plane_pline_3d.getPointAtLength(l) # point at s= l
    err+= (p-pr).getModulus()**2

err= math.sqrt(err) # average quadratic error

#print('err= ', err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
