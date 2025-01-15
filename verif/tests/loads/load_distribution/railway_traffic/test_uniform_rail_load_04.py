# -*- coding: utf-8 -*-
'''Railway bridge loads. Distributiion of a rail uniform load due to braking.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from actions.railway_traffic import uniform_rail_load
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
        modelSpace.fixNode000_FFF(n.tag)

# Define load.
sleeperThickness= 22e-2
sleeperLength= 2.6
standardGauge= 1.435
lateralWidth= sleeperLength-standardGauge
sleeperSpreadToDepthRatio= lateralWidth/sleeperThickness

ballastThickness= 0.6 # Thickness of the ballast under the sleeper.
ballastSpreadToDepthRatio= 1/4.0 # See Eurocode 1 part 2 figure 6.5.

spreadingLayers= [(sleeperThickness, sleeperSpreadToDepthRatio), (ballastThickness, ballastSpreadToDepthRatio)]

## Define track axis.
axisPoints3d= list()
for p in axisPoints2d:
    axisPoints3d.append(to_3d(p, zDeck+ballastThickness+sleeperThickness))
axisPlanePolyline3d= geom.PlanePolyline3d(axisPoints3d)



## Rail axis.
trackGauge= 1.435
offsetDist= trackGauge/2.0
rail1= axisPlanePolyline3d.offset(-offsetDist)
rail2= axisPlanePolyline3d.offset(offsetDist)

load= 40e3
dynamicFactor= 1.2
classificationFactor= 1.0
uniformRailLoad1= uniform_rail_load.UniformRailLoad(rail1, load, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
uniformRailLoad2= uniform_rail_load.UniformRailLoad(rail2, load, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)

## Define load pattern.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
loadedSet= modelSpace.getTotalSet()
totalBrakingLoad= 6000e3
railBrakingLoad= totalBrakingLoad/2.0 # Half the brake load for each rail.
nodalLoads= uniformRailLoad1.defDeckRailBrakingLoadsThroughLayers(brakingLoad= railBrakingLoad, spreadingLayers= spreadingLayers, originSet= loadedSet, deckMidplane= deckMidplane, deckThickness= deckThickness)
nodalLoads.extend(uniformRailLoad2.defDeckRailBrakingLoadsThroughLayers(brakingLoad= railBrakingLoad, spreadingLayers= spreadingLayers, originSet= loadedSet, deckMidplane= deckMidplane, deckThickness= deckThickness))

modelSpace.addLoadCaseToDomain(lp0.name)

## Check total load value.
lIter= lp0.loads.getNodalLoadIter
nLoad=lIter.next()
totalLoad= 0.0
while(nLoad):
    vLoad= nLoad.getForce
    loadVector= xc.Vector([vLoad[0], vLoad[1], vLoad[2]])
    totalLoad+= loadVector.Norm()
    nLoad= lIter.next()

# totalLoad= totalLoadVector.Norm()

err= abs(totalLoad-totalBrakingLoad)/totalBrakingLoad

'''
print(totalLoad)
print(totalBrakingLoad)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)

