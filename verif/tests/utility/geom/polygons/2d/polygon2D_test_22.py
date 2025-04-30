# -*- coding: utf-8 -*-
'''Test Polygon2d intersection with half-space. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom

# Define polygon.
points= [geom.Pos2d(-6.55729e-06,-0.444992), geom.Pos2d(0.00088,-0.75), geom.Pos2d(0.192791,-0.75), geom.Pos2d(0.2165,-0.6615), geom.Pos2d(0.1875,-0.55324), geom.Pos2d(0.10825,-0.474), geom.Pos2d(0,-0.44499)]
plg= geom.Polygon2d(points)

refArea= plg.getArea()

# Define half-spaces.
pA= geom.Pos2d(-0.0826875,-1)
pB= geom.Pos2d(-0.0826875, 1)
ln= geom.Line2d(pA, pB)
halfPlaneA= geom.HalfPlane2d(ln, geom.Pos2d(100,0))
halfPlaneB= geom.HalfPlane2d(ln, geom.Pos2d(-100,0))

testOK= halfPlaneA.In(plg, 1e-3)
testOK= testOK and (not halfPlaneB.In(plg, 1e-3))

intA= plg.getIntersection(halfPlaneA)[0]
area= intA.getArea()
ratio1= abs(area-refArea)/refArea
testOK= testOK and (abs(ratio1)<1e-5)
intB= plg.getIntersection(halfPlaneB)
testOK= testOK and (len(intB)==0)

'''
print(testOK)
print(area, refArea)
print(ratio1)
print(intB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
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
# x,y= getXYVertices(plg)
# ax.plot(x,y,'tab:blue')
# # plg= getPolygon(plgInt)
# # ax.add_patch(plg)
# ax.plot([pA.x, pB.x], [pA.y, pB.y], marker= 'o')
# ax.set_aspect('equal', adjustable='box')
# plt.show()



