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

def getPoints(lstPos, nDiv):
    retval= list()
    p0= lstPos[0]
    for p in lstPos[1:]:
        sg= geom.Segment3d(p0,p)
        retval.extend(sg.Divide(nDiv)[:-1])
        p0= p
    sg= geom.Segment3d(lstPos[-1],lstPos[0])
    retval.extend(sg.Divide(nDiv)[:-1])
    return retval
    

corners= [geom.Pos3d(0,0,0), geom.Pos3d(10,0,0), geom.Pos3d(10,10,0), geom.Pos3d(0,10,0)]
holeACorners= [geom.Pos3d(4,4,0), geom.Pos3d(5,4,0), geom.Pos3d(5,5,0), geom.Pos3d(4,5,0)]
holeBCorners= [geom.Pos3d(1,3,0), geom.Pos3d(2,2,0), geom.Pos3d(3,3,0), geom.Pos3d(2,4,0)]

points= getPoints(corners, nDiv= 10)
holeAPoints= getPoints(holeACorners, nDiv= 1)
holeBPoints= getPoints(holeBCorners, nDiv= 1)

# # Draw polygon
# xs= list()
# ys= list()
# for p in points:
#     xs.append(p.x)
#     ys.append(p.y)
    
# xhA= list()
# yhA= list()
# for p in holeAPoints:
#     xhA.append(p.x)
#     yhA.append(p.y)
    
# xhB= list()
# yhB= list()
# for p in holeBPoints:
#     xhB.append(p.x)
#     yhB.append(p.y)

# plt.figure()
# plt.plot(xs,ys) 
# plt.scatter(xs, ys)
# plt.plot(xhA,yhA) 
# plt.scatter(xhA, yhA)
# plt.plot(xhB,yhB) 
# plt.scatter(xhB, yhB)
# plt.show()


plgExt= geom.Polygon3d(points)
plgIntA= geom.Polygon3d(holeAPoints)
plgIntB= geom.Polygon3d(holeBPoints)

paver= xc.Paver()

paver.mesh(plgExt,[plgIntA, plgIntB])

nodPos= paver.getNodePositions()
quads= paver.getQuads()



nnod= len(nodPos)
ratio1= (nnod-136)
nquads= len(quads)
ratio2= (nquads-88)

'''
print('ext. contour point number: ', len(points))
print('int. contour point number: ', len(holeAPoints))
print('area: ', plgExt.getArea())
print(nnod)
print(nquads)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0) & (ratio2==0) :
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')

'''    
# Draw elements
plg= list()
for q in quads:
    qx= list()
    qy= list()
    q.append(q[0])
    for i in q:
        pos= nodPos[i]
        qx.append(pos.x)
        qy.append(pos.y)
    plg.append((qx,qy))

plt.figure()
for p in plg:
    xs= p[0]
    ys= p[1]
    #plt.scatter(xs, ys)
    plt.fill(xs,ys) 
plt.show()
'''
