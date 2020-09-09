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
#from postprocess import output_handler

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

p1= geom.Pos3d(0,0,0)
p2= geom.Pos3d(10,0,0)
p3= geom.Pos3d(10,10,0)
p4= geom.Pos3d(0,10,0)

pH0= geom.Pos3d(4,4,0)
pH1= geom.Pos3d(5,4,0)
pH2= geom.Pos3d(5,5,0)
pH3= geom.Pos3d(4,5,0)

corners= [p1, p2, p3, p4]
holeCorners= [pH0, pH1, pH2, pH3]

points= getPoints(corners, nDiv= 10)
holePoints= getPoints(holeCorners, nDiv= 1)

'''
# Draw polygon
xs= list()
ys= list()
for p in points:
    xs.append(p.x)
    ys.append(p.y)
    
xh= list()
yh= list()
for p in holePoints:
    xh.append(p.x)
    yh.append(p.y)

plt.figure()
plt.plot(xs,ys) 
plt.scatter(xs, ys)
plt.plot(xh,yh) 
plt.scatter(xh, yh)
plt.show()
'''

plgExt= geom.Polygon3d(points)
plgInt= geom.Polygon3d(holePoints)

paver= xc.Paver()

paver.mesh(plgExt,[plgInt])

nodPos= paver.getNodePositions()
quads= paver.getQuads()

nNodPos= len(nodPos)
ratio1= (nNodPos-135)
nquads= len(quads)
ratio2= (nquads-99)

'''
# Draw quads
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
    #plt.plot(xs,ys) 
    plt.fill(xs,ys) 
plt.show()    
'''
# Test paving routine inside XC modeler.

## Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

### Define k-points.
points= modelSpace.getPointHandler()

#### Exterior contour
pt1= points.newPntFromPos3d(p1)
pt2= points.newPntFromPos3d(p2)
pt3= points.newPntFromPos3d(p3)
pt4= points.newPntFromPos3d(p4)

#### Hole
ptH0= points.newPntFromPos3d(pH0)
ptH1= points.newPntFromPos3d(pH1)
ptH2= points.newPntFromPos3d(pH2)
ptH3= points.newPntFromPos3d(pH3)

### Define polygonal surfaces
surfaces= modelSpace.getSurfaceHandler()
polyFace= surfaces.newPolygonalFacePts([pt1.tag, pt2.tag, pt3.tag, pt4.tag])
hole= surfaces.newPolygonalFacePts([ptH0.tag, ptH1.tag, ptH2.tag, ptH3.tag])

### Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

### Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

### Generate mesh.
polyFace.setNDiv(10)
hole.setNDiv(1)
polyFace.addHole(hole)
polyFace.genMesh(xc.meshDir.I)

xcTotalSet= modelSpace.getTotalSet()
nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

ratio3= (nNodes-nNodPos)
ratio4= (nElements-nquads)

'''
print('ext. contour point number: ', len(points))
print('int. contour point number: ', len(holePoints))
print('area: ', plgExt.getArea())
print(nNodPos)
print(nquads)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0) & (ratio2==0) & (ratio3==0) & (ratio4==0) :
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
