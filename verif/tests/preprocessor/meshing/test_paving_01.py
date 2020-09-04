# -*- coding: utf-8 -*-
'''Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
import matplotlib.pyplot as plt

corners= [geom.Pos3d(0,0,0), geom.Pos3d(1,0,0), geom.Pos3d(1,1,0), geom.Pos3d(0,1,0)]
nDiv= 8

points= list()
p0= corners[0]
for p in corners[1:]:
    sg= geom.Segment3d(p0,p)
    points.extend(sg.Divide(nDiv)[:-1])
    p0= p
sg= geom.Segment3d(corners[-1],corners[0])
points.extend(sg.Divide(nDiv)[:-1])

# # Draw polygon
# xs= list()
# ys= list()
# for p in points:
#     xs.append(p.x)
#     ys.append(p.y)

# plt.figure()
# plt.scatter(xs, ys)
# plt.plot(xs,ys) 
# plt.show()

plgExt= geom.Polygon3d(points)

paver= xc.Paver()

paver.mesh(plgExt,[])

nodPos= paver.getNodePositions()
quads= paver.getQuads()



nnod= len(nodPos)
ratio1= (nnod-81)
nquads= len(quads)
ratio2= (nquads-64)

'''
print('number of points: ', len(points))
print('area: ', plgExt.getArea())
print('number of nodes: ', nnod)
print('number of quads: ', nquads)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0) & (ratio2==0) :
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')

       
# # Draw elements
# plg= list()
# for q in quads:
#     qx= list()
#     qy= list()
#     q.append(q[0])
#     for i in q:
#         pos= nodPos[i]
#         qx.append(pos.x)
#         qy.append(pos.y)
#     plg.append((qx,qy))

# plt.figure()
# for p in plg:
#     xs= p[0]
#     ys= p[1]
#     #plt.scatter(xs, ys)
#     plt.plot(xs,ys) 
# plt.show()
