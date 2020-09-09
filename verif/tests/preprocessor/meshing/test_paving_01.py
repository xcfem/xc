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
from model import predefined_spaces
from materials import typical_materials
from postprocess import output_handler

# Test raw paving routine.
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

pos1= geom.Pos3d(0,0,0)
pos2= geom.Pos3d(1,0,0)
pos3= geom.Pos3d(1,1,0)
pos4= geom.Pos3d(0,1,0)

corners= [pos1, pos2, pos3, pos4]
nDiv= 8

points= getPoints(corners, nDiv= nDiv)

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



nNodPos= len(nodPos)
ratio1= (nNodPos-81)
nquads= len(quads)
ratio2= (nquads-64)

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


# Test paving routine inside XC modeler.

## Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

### Define k-points.
points= modelSpace.getPointHandler()

pt1= points.newPntFromPos3d(pos1)
pt2= points.newPntFromPos3d(pos2)
pt3= points.newPntFromPos3d(pos3)
pt4= points.newPntFromPos3d(pos4)

### Define polygonal surface
surfaces= modelSpace.getSurfaceHandler()
polyFace= surfaces.newPolygonalFacePts([pt1.tag, pt2.tag, pt3.tag, pt4.tag])

### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
polyFace.setNDiv(8)
polyFace.genMesh(xc.meshDir.I)

'''
print('number of points: ', len(points))
print('area: ', plgExt.getArea())
print('number of positions: ', nNodPos)
print('number of quads: ', nquads)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0) & (ratio2==0) :
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')

       
# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)

#oh.displayBlocks()#setToDisplay= )
#oh.displayFEMesh()#setsToDisplay=[])
#oh.displayLocalAxes()
#oh.displayLoads()
#oh.displayReactions()
#oh.displayDispRot(itemToDisp='uX')
#oh.displayDispRot(itemToDisp='uY')
#oh.displayDispRot(itemToDisp='uZ')
