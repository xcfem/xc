# -*- coding: utf-8 -*-
''' Point cloud oriented bounding box trivial test (2D).'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

def points_in_circumference(r= 1, n=20):
    return [geom.Pos2d(math.cos(2*math.pi/n*x)*r,math.sin(2*math.pi/n*x)*r) for x in range(0,n+1)]

xPoints= list()
for i in range(0,50):
    x= float(i)
    xPoints.append(geom.Pos2d(x, x))
    
mirrorXPoints= list()
for p in reversed(xPoints[1:]):
    mirrorXPoints.append(geom.Pos2d(-p.x, -p.y))
linePoints= mirrorXPoints+xPoints
circumferencePoints= points_in_circumference()
xPoints= linePoints+circumferencePoints


xPointCloud= geom.PointCloud2d(xPoints)

boundingBox= geom.get_oriented_bounding_box(xPointCloud)
boundingBoxVertices= boundingBox.getVertices()
referenceVertices= [geom.Pos2d(-48.3018, -49.6983), geom.Pos2d(49.6985, 48.3014), geom.Pos2d(48.3018, 49.6983), geom.Pos2d(-49.6985, -48.3014)]
err= 0.0
for v, vRef in zip(boundingBoxVertices, referenceVertices):
    err+= v.dist2(vRef)
err= math.sqrt(err)

print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-3):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# Graphic output
import matplotlib.pyplot as plt
def get_xy_vertices(vertices):
    x= list()
    y= list()
    for p in vertices:
        x.append(p.x)
        y.append(p.y)
    x.append(vertices[0].x)
    y.append(vertices[0].y)
    return x,y
    
fig = plt.figure()
ax = fig.add_subplot(111)
x, y= get_xy_vertices(vertices= boundingBox.getVertices())
ax.plot(x,y,'tab:gray')
x, y= get_xy_vertices(vertices= linePoints)
ax.plot(x,y,'tab:green')
x, y= get_xy_vertices(vertices= circumferencePoints)
ax.plot(x,y,'tab:blue')
plt.show()
