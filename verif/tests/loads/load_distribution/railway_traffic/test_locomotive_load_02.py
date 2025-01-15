# -*- coding: utf-8 -*-
'''Railway bridge loads. Distribution of the locomotive centrifugal loads.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from actions.railway_traffic import EC1_rail_load_models
from actions.railway_traffic import track_cross_section_geometry as tcs

# Problem geometry.
side= 15.0
p0= geom.Pos3d(0.0, 0.0, 0.0)
p1= geom.Pos3d(side, 0.0, 0.0)
p2= geom.Pos3d(side, side, 0.0)
p3= geom.Pos3d(0.0, side, 0.0)

deckMidplane= geom.Plane3d(p0, p1, p3)

# Define FE problem
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define FE problem geometry.
points= preprocessor.getMultiBlockTopology.getPoints
pt0= points.newPoint(p0)
pt1= points.newPoint(p1)
pt2= points.newPoint(p2)
pt3= points.newPoint(p3)
s= modelSpace.newSurface([pt0, pt1, pt2, pt3])
s.setElemSizeIJ(0.5,0.5)

## Define materials
E= 30e9 # Elastic modulus
nu= 0.2 # Poisson's ratio
deckThickness= 0.3 # Deck thickness.
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E= E, nu= nu, rho= 0.0, h= deckThickness)

## Define seed element.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4")

## Create mesh.
s.genMesh(xc.meshDir.I)

## Define constraints
for l in s.getSides: # Iterate through the perimeter.
    for i in l.getEdge.getNodeTags():
        modelSpace.fixNode000_FFF(i)

# Define load.
## Define spreading parameters.
sleeperThickness= 22e-2
sleeperLength= 2.6
standardGauge= 1.435
lateralWidth= sleeperLength-standardGauge
sleeperSpreadToDepthRatio= lateralWidth/sleeperThickness

ballastThickness= 0.3 # Thickness of the ballast under the sleeper.
ballastSpreadToDepthRatio= 1/4.0 # See Eurocode 1 part 2 figure 6.5.

spreadingLayers= [(sleeperThickness, sleeperSpreadToDepthRatio), (ballastThickness, ballastSpreadToDepthRatio)]

## Locomotive centrifugal load.

### Centrifugal load parameters.
#### Define speed, radius and influence length.
V= 200 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)
r= 2200 # radius of curvature.
Lf=  5.0 # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural element under consideration [m].
#### Define track cross-section parameters.
h= 2.8 # height of the train center of gravity with respect to the reference
       # plane.
s= 3.0 # exaggerated with demonstration purposes.
u= 0.15 # cant.
cs= tcs.TrackCrossSection(s= s, u= u)

### Locomotive load model.
#### Wheel spacing is exaggerated to get separate loads for each wheel
#### (only for demonstration purposes).
locomotiveLoad= EC1_rail_load_models.LocomotiveLoad(xSpacing= 3, ySpacing= 3.0, dynamicFactor= 1.0, classificationFactor= 1.21)
### Locomotive position and orientation.
zRail= ballastThickness+sleeperThickness
railUpVector= geom.Vector3d(0,0,zRail)
dirVector= p1-p0
midPoint= p0+0.5*(p2-p0)+railUpVector
normalVector= geom.Vector3d(-dirVector.y, dirVector.x, 0.0) 
ref= geom.Ref2d3d(midPoint, dirVector, normalVector)

locomotiveCentrifugalLoads= locomotiveLoad.getCentrifugalWheelLoads(v= v, Lf= Lf, r= r, trackCrossSection= cs, h= h)


## Define load pattern.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
loadedSet= modelSpace.getTotalSet()
nodalLoads= locomotiveLoad.defDeckCentrifugalWheelLoadsThroughLayers(centrifugalLoads= locomotiveCentrifugalLoads, centrifugalDirection= -ref.getJVector(), ref= ref, spreadingLayers= spreadingLayers, originSet= loadedSet, deckThickness= deckThickness, deckSpreadingRatio= 1/1)

modelSpace.addLoadCaseToDomain(lp0.name)

## Check total load value.
totalLoad= xc.Vector([0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
for nl in nodalLoads:
    totalLoad-= nl.getLoadVector # z component.

# Compute reference value.
totalCentrifugalLoad= locomotiveLoad.getCentrifugalLoadPerWheel(v, Lf, r)*locomotiveLoad.getNumWheels()
refTotalLoad= xc.Vector([0, totalCentrifugalLoad, 0, 0, 0, 0])


err= (totalLoad-refTotalLoad).Norm()/totalCentrifugalLoad

'''
print(totalLoad)
print(refTotalLoad)
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

