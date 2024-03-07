# -*- coding: utf-8 -*-
'''Railway bridge loads. Rail uniform load.'''

import math
import geom
import xc
from actions.railway_traffic import load_model_base
from model import predefined_spaces
from materials import typical_materials

def to_3d(p2d, z):
    return geom.Pos3d(p2d.x, p2d.y, z)

# Problem geometry.
width= 10.0
p0= geom.Pos2d(0.0, 0.0)
p1= geom.Pos2d(20.0, -3.0)
p2= geom.Pos2d(40.0, 1.0)
p3= geom.Pos2d(70.0, 7.0)
p4= geom.Pos2d(100.0, 0.0)

firstCurve= geom.CircularSector2d(p0, p1, p2)
secondCurve= geom.CircularSector2d(p2, p3, p4)

ptsA= firstCurve.getArcPoints(11)
ptsB= secondCurve.getArcPoints(11)

axisPoints2d= [p0]
for p in ptsA[1:-1]:
    axisPoints2d.append(p)
axisPoints2d.append(p2)
for p in ptsB[1:-1]:
    axisPoints2d.append(p)
axisPoints2d.append(p4)

axisPolyline2d= geom.Polyline2d(axisPoints2d)
pointsLeftBorder2d= axisPolyline2d.offset(-width/2.0).getVertexList()
pointsRightBorder2d= axisPolyline2d.offset(width/2.0).getVertexList()

# Define FE problem
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
points= preprocessor.getMultiBlockTopology.getPoints

## Define problem geometry.
quadPoints= list()
zDeck= 5.0
elemSize= 1.0
pI0= pointsLeftBorder2d[0]
pi03d= geom.Pos3d(pI0.x, pI0.y, zDeck)
ptI0= points.newPoint(pi03d)
pD0= pointsRightBorder2d[0]
pd03d= geom.Pos3d(pD0.x, pD0.y, zDeck)
ptD0= points.newPoint(pd03d)
deckPoints3d= [pi03d, pd03d]
leftKPoints= [ptI0]
rightKPoints= [ptD0]
slabSet= modelSpace.defSet('slabSet')
for pI, pD in zip(pointsLeftBorder2d[1:], pointsRightBorder2d[1:]):
    pi3d= geom.Pos3d(pI.x, pI.y, zDeck)
    ptI= points.newPoint(pi3d)
    deckPoints3d.append(pi3d)
    pd3d= geom.Pos3d(pD.x, pD.y, zDeck)
    ptD= points.newPoint(pd3d)
    deckPoints3d.append(pd3d)
    s= modelSpace.newSurface([ptD0, ptD, ptI, ptI0])
    s.setElemSizeIJ(elemSize, elemSize)
    slabSet.surfaces.append(s)
    leftKPoints.append(ptI)
    rightKPoints.append(ptD)
    ptI0= ptI
    ptD0= ptD

deckMidplane= geom.Plane3d(deckPoints3d)
slabSet.fillDownwards()

### Conciliate the number of divisions of the lines.
modelSpace.conciliaNDivs()

## Define materials
E= 30e9 # Elastic modulus
nu= 0.2 # Poisson's ratio
deckThickness= 1.0 # Deck thickness.
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E= E, nu= nu, rho= 0.0, h= deckThickness)

## Define seed element.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4")

## Mesh generation. 
slabSet.genMesh(xc.meshDir.I)
slabSet.fillDownwards()

## Define constraints.
constrainedNodeTagsA= list()
constrainedLines= list()
### Abutments.
l= slabSet.lines.getNearest(to_3d(axisPoints2d[0], zDeck))
constrainedLines.append(l)
l= slabSet.lines.getNearest(to_3d(axisPoints2d[-1], zDeck))
constrainedLines.append(l)

length= 0.0
span= 10.0
p0= axisPoints2d[0]
for p in axisPoints2d[1:-1]:
    length+= p.dist(p0)
    if(length>=span):
        p3d= to_3d(p, zDeck)
        l= slabSet.lines.getNearest(p3d)
        constrainedLines.append(l)
        length= 0.0 # start again.
    p0= p

for l in constrainedLines:
    for n in l.nodes:
        if n.tag not in constrainedNodeTagsA:
            modelSpace.fixNode000_FFF(n.tag)
            constrainedNodeTagsA.append(n.tag)


# Retrieve the constrained nodes.
nodeTagsOK= True
constrainedNodeTagsB= list()
xcTotalSet= modelSpace.getTotalSet()
for n in xcTotalSet.nodes:
    constraints= n.getConnectedConstraints()
    if(len(constraints)>0):
        constrainedNodeTagsB.append(n.tag)
        for c in constraints:
            # print(c.tipo())
            if(c.nodeTag!=n.tag):
                nodeTagsOK= False

# Check the results.
diffBA= list()
for tag in constrainedNodeTagsB:
    if tag not in constrainedNodeTagsA:
        diffBA.append(tag)
        
diffAB= list()
for tag in constrainedNodeTagsA:
    if tag not in constrainedNodeTagsB:
        diffAB.append(tag)

'''
print(constrainedNodeTagsA)
print(constrainedNodeTagsB)
print(diffAB, diffBA)
'''

testOK= nodeTagsOK and (len(diffAB)==0) and (len(diffBA)==0)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
