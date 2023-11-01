# -*- coding: utf-8 -*-
''' Polygon partition verification test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

referencePerimeter=4+math.sqrt(2)

plg1=geom.Polygon2d([geom.Pos2d(-1.,-1.), geom.Pos2d(1.,-1.), geom.Pos2d(1.,1.), geom.Pos2d(-1.,1.)])

plg2=geom.Polygon2d(plg1)
plg2.move(geom.Vector2d(1,1))

plg3=geom.Polygon2d([geom.Pos2d(0.,-1.), geom.Pos2d(1.,-1.), geom.Pos2d(1.,2.), geom.Pos2d(0.,2.)])

list1=[plg1,plg2]
list1=geom.particiona(list1)
list1=geom.clip(list1,plg3)

p1=list1[0]
ratio1=(p1.getPerimeter()-referencePerimeter)/referencePerimeter

p2=list1[1]
ratio2=(p2.getPerimeter()-referencePerimeter)/referencePerimeter

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
    
# # Graphic output
# import matplotlib.pyplot as plt
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
    
# fig = plt.figure()
# ax = fig.add_subplot(111)
# x,y= getXYVertices(plg1)
# ax.plot(x,y,'tab:gray')
# x,y= getXYVertices(plg2)
# plt.plot(x, y, 'tab:blue')
# x,y= getXYVertices(plg3)
# plt.plot(x, y, 'tab:red')
# plt.show()
