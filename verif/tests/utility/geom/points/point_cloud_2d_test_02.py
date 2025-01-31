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

xPoints= [geom.Pos2d(-1, -1), geom.Pos2d(1,1), geom.Pos2d(-1,1)]



xPointCloud= geom.PointCloud2d(xPoints)

boundingBox= geom.get_oriented_bounding_box(xPointCloud)
boundingBoxVertices= boundingBox.getVertices()
referenceVertices= [xPoints[0], xPoints[1], geom.Pos2d(0,2), geom.Pos2d(-2,0)]
err= 0.0
for v, vRef in zip(boundingBoxVertices, referenceVertices):
    err+= v.dist2(vRef)
err= math.sqrt(err)

# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Graphic output
# import matplotlib.pyplot as plt
# def get_xy_vertices(vertices):
#     x= list()
#     y= list()
#     for p in vertices:
#         x.append(p.x)
#         y.append(p.y)
#     x.append(vertices[0].x)
#     y.append(vertices[0].y)
#     return x,y
    
# fig = plt.figure()
# ax = fig.add_subplot(111)
# x, y= get_xy_vertices(vertices= boundingBox.getVertices())
# ax.plot(x,y,'tab:red')
# x, y= get_xy_vertices(vertices= xPoints)
# ax.plot(x,y,'tab:green')
# plt.show()
