# -*- coding: utf-8 -*-
'''Polygon2D test. Clip a polygon using another one.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
plg1=geom.Polygon2d([geom.Pos2d(-1.,-1.), geom.Pos2d(1.,-1.), geom.Pos2d(1.,1.), geom.Pos2d(-1.,1.)])

plg2=geom.Polygon2d(plg1)
plg2.move(geom.Vector2d(1,1))

plg3=geom.Polygon2d(plg1)
plg3.clipUsingPolygon(plg2)

areaPol=plg3.getArea()
polygonPerimeter=plg3.getPerimeter()
polygonCenterOfMass=plg3.getCenterOfMass()
polygonCenterOfMassX=polygonCenterOfMass.x
polygonCenterOfMassY=polygonCenterOfMass.y
IxPol=plg3.getIx()
IyPol=plg3.getIy()
PxyPol=plg3.getPxy()

ratio1= (areaPol-1)
ratio2= (polygonPerimeter-4)/4.
ratio3= (polygonCenterOfMassX-0.5)/0.5
ratio4= (polygonCenterOfMassY-0.5)/0.5
ratio5= (IxPol-(1/12.0))/(1/12.0)
ratio6= (IyPol-(1/12.0))/(1/12.0)
ratio7= PxyPol

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5)<1e-10 and abs(ratio6)<1e-10 and abs(ratio7)<1e-10:
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
