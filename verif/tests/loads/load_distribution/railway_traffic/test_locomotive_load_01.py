# -*- coding: utf-8 -*-
'''Railway bridge loads. Rail uniform load.'''


import math
import geom
import xc
from actions.railway_traffic import locomotive_load
from model import predefined_spaces
from materials import typical_materials

# Problem geometry.
side= 15.0
p0= geom.Pos3d(0.0, 0.0, 0.0)
p1= geom.Pos3d(side, 0.0, 0.0)
p2= geom.Pos3d(side, side, 0.0)
p3= geom.Pos3d(0.0, side, 0.0)

deckMidplane= geom.Plane3d(p0, p1, p3)

# Define FE problem
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
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
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

## Create mesh.
s.genMesh(xc.meshDir.I)

## Define constraints
for l in s.getSides: # Iterate through the perimeter.
    for i in l.getEdge.getNodeTags():
        modelSpace.fixNode000_FFF(i)

# Define load.
sleeperThickness= 22e-2
sleeperLength= 2.6
standardGauge= 1.435
lateralWidth= sleeperLength-standardGauge
sleeperSpreadToDepthRatio= lateralWidth/sleeperThickness

ballastThickness= 0.3 # Thickness of the ballast under the sleeper.
ballastSpreadToDepthRatio= 1/4.0 # See Eurocode 1 part 2 figure 6.5.

spreadingLayers= [(sleeperThickness, sleeperSpreadToDepthRatio), (ballastThickness, ballastSpreadToDepthRatio)]

## Locomotive load.
### Locomotive load model.
dynamicFactor= 1.2
classificationFactor= 1.0
#### Wheel spacing is exaggerated to get separate loads for each wheel
#### (only for demonstration purposes).
locomotiveLoad= locomotive_load.LocomotiveLoad(xSpacing= 3, ySpacing= 3.0, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
### Locomotive position and orientation.
zRail= ballastThickness+sleeperThickness
railUpVector= geom.Vector3d(0,0,zRail)
dirVector= p1-p0
midPoint= p0+0.5*(p2-p0)+railUpVector
normalVector= geom.Vector3d(-dirVector.y, dirVector.x, 0.0) 
ref= geom.Ref2d3d(midPoint, dirVector, normalVector)

## Define load pattern.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
loadedSet= modelSpace.getTotalSet()
nodalLoads= locomotiveLoad.defDeckWheelLoadsThroughLayers(ref= ref, spreadingLayers= spreadingLayers, originSet= loadedSet, deckThickness= deckThickness, deckSpreadingRatio= 1/1)

modelSpace.addLoadCaseToDomain(lp0.name)

## Check total load value.
totalLoad= 0.0
for nl in nodalLoads:
    totalLoad-= nl.getLoadVector[2] # z component.

refTotalLoad= locomotiveLoad.getTotalLoad()

err= abs(totalLoad-refTotalLoad)/refTotalLoad

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

