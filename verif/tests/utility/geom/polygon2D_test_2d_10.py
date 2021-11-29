# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

import xc_base
import geom
# import matplotlib.pyplot as plt

pol=geom.Polygon2d()

#        +----+
#        |    |
#        |    |
#   +----+    +----+
#   |              |
#   |              |
#   +----+    +----+
#        |    |
#        |    |
#        +----+

pol.appendVertex(geom.Pos2d(1,0))
pol.appendVertex(geom.Pos2d(2,0))
pol.appendVertex(geom.Pos2d(2,1))
pol.appendVertex(geom.Pos2d(3,1))
pol.appendVertex(geom.Pos2d(3,2))
pol.appendVertex(geom.Pos2d(2,2))
pol.appendVertex(geom.Pos2d(2,3))
pol.appendVertex(geom.Pos2d(1,3))
pol.appendVertex(geom.Pos2d(1,2))
pol.appendVertex(geom.Pos2d(0,2))
pol.appendVertex(geom.Pos2d(0,1))
pol.appendVertex(geom.Pos2d(1,1))
area= pol.getArea()


decomp= pol.getBayazitDecomposition()

for p in decomp:
  a= p.getArea()
  area-= a

#print('area= ',area)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(area)<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

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
# x,y= getXYVertices(pol)
# ax.fill(x,y,'tab:gray')
# colors= ['tab:blue', 'tab:orange', 'tab:green', 'tab:red', 'tab:purple', 'tab:brown', 'tab:pink', 'tab:gray', 'tab:olive', 'tab:cyan']
# i= 0
# for plg in decomp:
#     x,y= getXYVertices(plg)
#     ax.plot(x,y,colors[i])
#     i+=1
  
# plt.show()


