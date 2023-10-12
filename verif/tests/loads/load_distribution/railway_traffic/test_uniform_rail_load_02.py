# -*- coding: utf-8 -*-
'''Railway bridge loads. Rail uniform load.'''

import math
import geom
import xc
from actions.railway_traffic import uniform_rail_load
from model import predefined_spaces
from materials import typical_materials

# Problem geometry.
side= 10.0
p0= geom.Pos3d(0.0, 0.0, 0.0)
p1= geom.Pos3d(side, 0.0, 0.0)
p2= geom.Pos3d(side, side, 0.0)
p3= geom.Pos3d(0.0, side, 0.0)

pA= geom.Pos3d(side/2.0, side/2.0, 0.0)

deckMidplane= geom.Plane3d(p0, p1, p3)

# Define FE problem
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define problem geometry.
points= preprocessor.getMultiBlockTopology.getPoints
pt0= points.newPoint(p0)
pt1= points.newPoint(p1)
pt2= points.newPoint(p2)
pt3= points.newPoint(p3)
s= modelSpace.newSurface([pt0, pt1, pt2, pt3])
s.setElemSizeIJ(0.5,0.5)
deckThickness= 0.3

## Define materials
E= 20e9 # Elastic modulus
nu= 0.2 # Poisson's ratio
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

ballastThickness= 0.6 # Thickness of the ballast under the sleeper.
ballastSpreadToDepthRatio= 1/4.0 # See Eurocode 1 part 2 figure 6.5.

spreadingLayers= [(sleeperThickness, sleeperSpreadToDepthRatio), (ballastThickness, ballastSpreadToDepthRatio)]

## Rail axis.
zRail= ballastThickness+sleeperThickness
railUpVector= geom.Vector3d(0,0,zRail)
railAxis= geom.Polyline3d([p0+railUpVector, pA+railUpVector, p2+railUpVector])
load= 40e3
dynamicFactor= 1.2
classificationFactor= 1.0
uniformRailLoad= uniform_rail_load.UniformRailLoad(railAxis, load, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)

## Define load pattern.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
loadedSet= modelSpace.getTotalSet()
nodalLoads= uniformRailLoad.defDeckRailUniformLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= loadedSet, deckMidplane= deckMidplane, deckThickness= deckThickness)

modelSpace.addLoadCaseToDomain(lp0.name)

## Check total load value.
totalLoad= 0.0
for nl in nodalLoads:
    totalLoad-= nl.getLoadVector[2] # z component.

refTotalLoad= uniformRailLoad.getDynamicLoad()*uniformRailLoad.railAxis.getLength()

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

