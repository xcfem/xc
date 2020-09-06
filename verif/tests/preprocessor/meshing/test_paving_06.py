# -*- coding: utf-8 -*-
'''Gusset plate mesh generation using paving algorithm. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
import matplotlib.pyplot as plt
from materials.astm_aisc import ASTM_materials

def round_up_to_even(f):
    return math.ceil(f / 2.) * 2

def getIntermediatePoints(corners, elemSide):
    ''' Return a series of intermediate points uniformly distributed
        on the sides of the polygon argument.

        corners: list of point that define the original contour.
        elemSide: length of the element side.
    ''' 
    pts= list()
    p0= corners[0]
    for p in corners[1:]:
        sg= geom.Segment3d(p0,p)
        nDiv= int(round_up_to_even(sg.getLength()/elemSide))
        pts.extend(sg.Divide(nDiv)[:-1])
        p0= p
    sg= geom.Segment3d(corners[-1],corners[0])
    nDiv= int(round_up_to_even(sg.getLength()/elemSide))
    pts.extend(sg.Divide(nDiv)[:-1])
    return geom.Polygon3d(pts)

def drawPolygons(plgList):
    ''' Draws the polygons on the argument list using matplotlib.

    plgList: list containing the polygons to draw.
    '''
    x= list()
    y= list()
    for plg in plgList:
        vtx= plg.getVertexList()
        xi= list()
        yi= list()
        for v in vtx:
            xi.append(v.x)
            yi.append(v.y)
        x.append(xi)
        y.append(yi)
    plt.figure()
    for xi, yi in zip(x,y):
        plt.plot(xi,yi) 
        plt.scatter(xi, yi)
    plt.axis('equal')
    plt.show()
    
def drawMesh(nodes, quads):
    ''' Draw the quad mesh using matplotlib.

    :param nodes: list of the positions of the nodes.
    :param quads: index of the element nodes in the
                  positions list.
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
    plt.axis('equal')
    plt.show()

corners= [geom.Pos3d(-1.5950,-2.0450,0), geom.Pos3d(-1.4376,-2.0450,0), geom.Pos3d(-1.4376,-1.8140,0), geom.Pos3d(-1.5290,-1.7734,0), geom.Pos3d(-1.5950, -1.7734, 0)]

bolts= list()

boltDiameter= 0.016
bolts.append(ASTM_materials.BoltFastener(diameter= boltDiameter, pos3d= geom.Pos3d(-1.4909,-1.8724,0.0)))
bolts.append(ASTM_materials.BoltFastener(diameter= boltDiameter, pos3d= geom.Pos3d(-1.4706,-1.8267,0.0)))
bolts.append(ASTM_materials.BoltFastener(diameter= boltDiameter, pos3d= geom.Pos3d(-1.5163,-1.8064,0.0)))
bolts.append(ASTM_materials.BoltFastener(diameter= boltDiameter, pos3d= geom.Pos3d(-1.5366,-1.8521,0.0)))

plgExt= getIntermediatePoints(corners, elemSide= 0.75*boltDiameter)
contours= [plgExt]
holes= list()
for b in bolts:
    plg= b.getHoleAsPolygon()
    holes.append(plg)
    contours.append(plg)

# drawPolygons(contours)

# Meshing with paving algorithm
paver= xc.Paver()
paver.mesh(plgExt, holes)
nodPos= paver.getNodePositions()
quads= paver.getQuads()

# drawMesh(nodPos, quads)

nnod= len(nodPos)
ratio1= (nnod-521)
nquads= len(quads)
ratio2= (nquads-400)

'''
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

