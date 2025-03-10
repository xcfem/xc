# -*- coding: utf-8 -*-
'''Railway bridge loads. Distribution of the centrifugal load derived from the uniform loads over a track axis.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from actions.railway_traffic import EC1_rail_load_models
from actions.railway_traffic import uniform_rail_load as url
from actions.railway_traffic import track_cross_section_geometry as tcs
from model import predefined_spaces
from materials import typical_materials

def to_3d(p2d, z):
    return geom.Pos3d(p2d.x, p2d.y, z)

# Problem geometry.
width= 10.0
p0= geom.Pos2d(-149.7423, -8.7891)
p1= geom.Pos2d(0.0, 0.0)
p2= geom.Pos2d(149.7423, -8.7891)

# Circle arc R= 1280 m, length: ~300.1719 m 
curveAxis= geom.CircularSector2d(p0, p1, p2)

axisPoints2d= curveAxis.getArcPoints(11)

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
locomotiveLM1= EC1_rail_load_models.get_locomotive_LM1()
trainLoadModel= EC1_rail_load_models.TrainLoadModel(locomotive= locomotiveLM1, uniformLoad= 80e3, dynamicFactor= 1.0, classificationFactor= 1.21)

## Define centrifugal load.

### Define spreading parameters.
sleeperThickness= 22e-2
sleeperLength= 2.6
standardGauge= 1.435
lateralWidth= sleeperLength-standardGauge
sleeperSpreadToDepthRatio= lateralWidth/sleeperThickness

ballastThickness= 0.8 # Thickness of the ballast under the sleeper.
ballastSpreadToDepthRatio= 1/4.0 # See Eurocode 1 part 2 figure 6.5.

spreadingLayers= [(sleeperThickness, sleeperSpreadToDepthRatio), (ballastThickness, ballastSpreadToDepthRatio)]

#### Rail axis.
leftRailAxisPoints= [geom.Pos3d(-149.8262, -8.0765, 0), geom.Pos3d(-122.6781, -5.1716, 0), geom.Pos3d(-95.4741, -2.8461, 0), geom.Pos3d(-68.2268, -1.1011, 0), geom.Pos3d(-40.9485, 0.0627, 0), geom.Pos3d(-13.6516, 0.6447, 0), geom.Pos3d(13.6516, 0.6447, 0), geom.Pos3d(40.9485, 0.0627, 0), geom.Pos3d(68.2268, -1.1011, 0), geom.Pos3d(95.4741, -2.8461, 0), geom.Pos3d(122.6781, -5.1716, 0), geom.Pos3d(149.8262, -8.0765, 0)]
leftRailAxis= geom.Polyline3d(leftRailAxisPoints)
#### Track cross section
trackCrossSection= tcs.TrackCrossSection(s= 1.435+.04, u= 0.0)

### Centrifugal load parameters.
#### Define speed, radius and influence length.
V= 160 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)
r= 1280.0 # radius of curvature.
Lf=  trackAxis.getLength() # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural element under consideration [m].
#### Uniform centrifugal load on the rails.
railCentrifugalLoadsPerMeter= trainLoadModel.getCentrifugalLoadPerMeter(v= v, Lf= Lf, r= r, trackCrossSection= trackCrossSection)
loadComponents=  [0.0, -railCentrifugalLoadsPerMeter[0].x, -railCentrifugalLoadsPerMeter[0].y]
centrifugalRailLoad= url.VariableDirectionRailLoad(railAxis= leftRailAxis, loadComponents= loadComponents, dynamicFactor= 1.0, classificationFactor= trainLoadModel.getClassificationFactor())
deckMidplane= slabSet.nodes.getRegressionPlane(0.0)

### Define load pattern.
q1a= modelSpace.newLoadPattern(name= 'Q1a')
modelSpace.setCurrentLoadPattern(q1a.name)
#### Define nodal loads.
nodalLoads= centrifugalRailLoad.defDeckRailLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= slabSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= 1/1)

modelSpace.addLoadCaseToDomain(q1a.name)

## Check total load value.
lIter= q1a.loads.getNodalLoadIter
totalLoad= xc.Vector([0.0, 0.0, 0.0])
totalLoadNorm= 0.0
nLoad=lIter.next()
while(nLoad):
    vLoad= nLoad.getForce
    loadVector= xc.Vector([vLoad[0], vLoad[1], vLoad[2]])
    totalLoad+= loadVector
    totalLoadNorm+= loadVector.Norm()
    nLoad= lIter.next()

totalLoadNormRef= math.sqrt((loadComponents[1])**2+(loadComponents[2])**2)*leftRailAxis.getLength()

err1= abs(totalLoadNorm-totalLoadNormRef)/totalLoadNormRef
err2= abs(totalLoad[0]/totalLoadNormRef)

'''
print('left rail axis length= ', leftRailAxis.getLength())
print('load components: ', loadComponents[1], loadComponents[2])
print('total load: ', totalLoad*1e-3, ' norm: ', totalLoad.Norm()*1e-3)
print('total load norm: ', totalLoadNorm/1e3)
print('reference total load norm: ',totalLoadNormRef/1e3)
print('err1: ', err1)
print('err2: ', err2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err1<1e-8 and err2<.005):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
