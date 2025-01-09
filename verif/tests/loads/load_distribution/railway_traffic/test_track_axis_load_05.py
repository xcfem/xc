# -*- coding: utf-8 -*-
'''Railway bridge loads. Centrifugal loads over a track axis.'''

import math
import geom
import xc
from actions.railway_traffic import EC1_rail_load_models
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
ptI0= points.newPoint(geom.Pos3d(pI0.x, pI0.y, zDeck))
pD0= pointsRightBorder2d[0]
ptD0= points.newPoint(geom.Pos3d(pD0.x, pD0.y, zDeck))
leftKPoints= [ptI0]
rightKPoints= [ptD0]
slabSet= modelSpace.defSet('slabSet')
for pI, pD in zip(pointsLeftBorder2d[1:], pointsRightBorder2d[1:]):
    ptI= points.newPoint(geom.Pos3d(pI.x, pI.y, zDeck))
    ptD= points.newPoint(geom.Pos3d(pD.x, pD.y, zDeck))
    s= modelSpace.newSurface([ptD0, ptD, ptI, ptI0])
    s.setElemSizeIJ(elemSize, elemSize)
    slabSet.surfaces.append(s)
    leftKPoints.append(ptI)
    rightKPoints.append(ptD)
    ptI0= ptI
    ptD0= ptD

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
## Define spreading parameters.
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
axisPolyline3d= geom.Polyline3d(axisPoints3d)

trackAxis= EC1_rail_load_models.TrackAxis(trackAxis= axisPolyline3d)
## Define train model.
trainLoadModel= EC1_rail_load_models.TrainLoadModel(locomotive= EC1_rail_load_models.locomotiveLM1, uniformLoad= 80e3, dynamicFactor= 1.0, classificationFactor= 1.21)

## Define centrifugal load.

### Centrifugal load parameters.
#### Define speed, radius and influence length.
V= 200 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)
r= 2200 # radius of curvature.
Lf=  trackAxis.getLength() # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural element under consideration [m].

## Define load pattern.
q1a= modelSpace.newLoadPattern(name= 'Q1a')
modelSpace.setCurrentLoadPattern(q1a.name)
relativePosition= 0.8
nodalLoads= trackAxis.defDeckCentrifugalLoadThroughLayers(trainModel= trainLoadModel, relativePosition= relativePosition, v= v, Lf= Lf, r= r, spreadingLayers= spreadingLayers, deckThickness= deckThickness, deckSpreadingRatio= 1/1, originSet= slabSet)
modelSpace.addLoadCaseToDomain(q1a.name)

## Check total load value.
totalLoad= geom.SlidingVectorsSystem3d()
totalLoadNorm= 0.0
lIter= q1a.loads.getNodalLoadIter
nLoad=lIter.next()
while(nLoad):
    vLoad= nLoad.getForce
    pos= nLoad.getNode.getInitialPos3d
    loadVector= geom.Vector3d(vLoad[0], vLoad[1], vLoad[2])
    totalLoad+= geom.SlidingVector3d(pos,loadVector)
    totalLoadNorm+= vLoad.Norm()
    nLoad= lIter.next()

# totalLoadRef= geom.SlidingVectorsSystem3d(geom.Pos3d(0,0,0), geom.Vector3d(26.0284e3, 190.515e3, -3.54625e3), geom.Vector3d(-1.42484e+06,477469,3.1818e+07))
# Values have changed after the Eigen 3 headers (which CGAL also uses) have been
# added to the source tree. Eigen 3 libraries are required by the Spectra libraries that are used in XC for eigenvalue computation.
# totalLoadRef= geom.SlidingVectorsSystem3d(geom.Pos3d(0,0,0), geom.Vector3d(19754.8, 163256, 497.355), geom.Vector3d(-1.19714e+06,267081,2.65238e+07))
# 20250109: reference values must change after fixing the error in
# getDeckLoadedContourThroughLayers method of RailRoadBase class
# see commit https://github.com/xcfem/xc/commit/7ed09e6b71c1c63b86547559312b669375fdf4f1
totalLoadRef= geom.SlidingVectorsSystem3d(geom.Pos3d(0,0,0), geom.Vector3d(19329.8, 160786, -3275.2), geom.Vector3d(-1.135e+06,391703,2.64643e+07))

test= totalLoad-totalLoadRef
R= test.getResultant()
M= test.getMoment()
errR= R.getModulus()/totalLoadRef.getResultant().getModulus() # Check the load values.
errM= M.getModulus()/totalLoadRef.getMoment().getModulus() # Check the correct placement of the loads.

'''
print('relative position: ', relativePosition)
print('total load: ', totalLoad)
print('reference load: ', totalLoadRef)
print('total load norm: ', totalLoadNorm/1e3)
print('R= ', R)
print('M= ', M)
print('errR: ', errR)
print('errM: ', errM)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(errR<1e-4 and errM<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# # qoh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
