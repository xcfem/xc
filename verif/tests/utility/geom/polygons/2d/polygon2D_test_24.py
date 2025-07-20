# -*- coding: utf-8 -*-
'''Test computation of principal axes of inertia for Polygon2d objects.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

# Define arbitrary polygon.
points=  [geom.Pos2d(7.9911, 86.8179), geom.Pos2d(-3.9881, 102.8334), geom.Pos2d(-7.5916, 100.1381), geom.Pos2d(-7.1311, 117.1887), geom.Pos2d(-6.8043, 117.428), geom.Pos2d(-0.8018, 114.4934), geom.Pos2d(6.005, 105.4055), geom.Pos2d(7.1372, 98.8107), geom.Pos2d(7.9911, 86.8179)]
plg= geom.Polygon2d(points)

plgPrincipalAxes= plg.getPrincipalAxes()

org= plgPrincipalAxes.Org
I1= plgPrincipalAxes.I1
I2= plgPrincipalAxes.I2

axis1= plgPrincipalAxes.getAxis1VDir()
axis2= plgPrincipalAxes.getAxis2VDir()

# Compute equivalent rectangle.
b= math.pow(12**2*I1**3/I2, 1/8.0)
a= 12*I1/b**3
p1= org+(-a/2.0*axis1-b/2.0*axis2)
p2= org+(a/2.0*axis1-b/2.0*axis2)
p3= org+(a/2.0*axis1+b/2.0*axis2)
p4= org+(-a/2.0*axis1+b/2.0*axis2)
rectg= geom.Polygon2d([p1, p2, p3, p4])
rectgPrincipalAxes= rectg.getPrincipalAxes()
rectgOrg= rectgPrincipalAxes.Org
rectgI1= rectgPrincipalAxes.I1
rectgI2= rectgPrincipalAxes.I2

rectgAxis1= rectgPrincipalAxes.getAxis1VDir()
rectgAxis2= rectgPrincipalAxes.getAxis2VDir()

ratio0= org.dist(rectgOrg)
ratio1= abs(I1-rectgI1)/rectgI1
ratio2= abs(I2-rectgI2)/rectgI2
ratio3= (axis1-rectgAxis1).getModulus()
ratio4= (axis2-rectgAxis2).getModulus()

'''
print('b= ', b, 'm')
print('a= ', a, 'm')
print('org= ', org)
print('rectgOrg= ', rectgOrg)
print('ratio0= ', ratio0)
print('I1= ', I1, 'm4')
print('rectgI1= ', rectgI1, 'm4')
print('ratio1= ', ratio1)
print('I2= ', I2, 'm4')
print('rectgI2= ', rectgI2, 'm4')
print('ratio2= ', ratio2)
print('axis1= ', axis1)
print('rectgAxis1= ', rectgAxis1)
print('ratio3= ', ratio3)
print('axis2= ', axis2)
print('rectgAxis2= ', rectgAxis2)
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-10 and abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10:
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
# plg2= getPolygon(rectg)
# ax.add_patch(plg2)
# ax.set_aspect('equal', adjustable='box')
# plt.show()
