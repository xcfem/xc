# -*- coding: utf-8 -*-
'''Test Polygon2d intersection. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

# Define polygons.
pointsA= [geom.Pos2d(0.666506,0.1205), geom.Pos2d(0.6375,0.228753), geom.Pos2d(0.558253,0.308), geom.Pos2d(0.45,0.337006), geom.Pos2d(0.341747,0.308), geom.Pos2d(0.2625,0.228753), geom.Pos2d(0.233494,0.1205), geom.Pos2d(0.2625,0.0122468), geom.Pos2d(0.341747,-0.067), geom.Pos2d(0.45,-0.0960064), geom.Pos2d(0.558253,-0.067), geom.Pos2d(0.6375,0.0122468)]
plgA= geom.Polygon2d(pointsA)

pointsB= [geom.Pos2d(-0.5,-0.175), geom.Pos2d(0.5,-0.175), geom.Pos2d(0.5,0.175), geom.Pos2d(-0.5,0.175)]
plgB= geom.Polygon2d(pointsB)

# Compute intersection.
plgInt= plgA.getIntersection(plgB)[0]

area= plgInt.getArea()
refArea= 0.05977376474018477
ratio1= abs(area-refArea)/refArea
perimeter= plgInt.getPerimeter()
refPerimeter= 0.9539145097877344
ratio2= abs(perimeter-refPerimeter)/refPerimeter

'''
print(plgInt)
print(area, ratio1)
print(perimeter, ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Graphic output
# import matplotlib.pyplot as plt
# from matplotlib.patches import Polygon
# def getXYVertices(plg):
#     x= list()
#     y= list()
#     vertices= plg.getVertices()
#     for p in vertices:
#         x.append(p.x)
#         y.append(p.y)
#     x.append(vertices[0].x)
#     y.append(vertices[0].y)
#     return x,y
# def getPolygon(plg):
#     pts= list()
#     vertices= plg.getVertices()
#     for p in vertices:
#         pts.append((p.x, p.y))
#     pts.append((vertices[0].x, vertices[0].y))
#     return Polygon(pts)
    
# fig = plt.figure()
# ax = fig.add_subplot(111)
# x,y= getXYVertices(plgA)
# ax.plot(x,y,'tab:blue')
# x,y= getXYVertices(plgB)
# ax.plot(x,y,'tab:red')
# plg= getPolygon(plgInt)
# ax.add_patch(plg)
# ax.set_aspect('equal', adjustable='box')
# plt.show()



