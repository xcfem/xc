# -*- coding: utf-8 -*-
''' Check gravity load of a concrete box.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from scipy.constants import g
from actions import load_cases as lcm

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry.
# 
#         p0              p3
#           +------------+
#           |            |
#           |            |
#           |            |
#           +------------+
#         p1              p2
#

wallThickness= 0.4
slabThickness= 0.5
boxWidth= 3.0
boxHeight= 2.0
slabLength= boxWidth+2.0*wallThickness
halfWidth= (boxWidth+wallThickness)/2.0
halfHeight= (boxHeight+slabThickness)/2.0
## Box volume (per unit length).
boxVolume= 2*wallThickness*boxHeight+2*slabThickness*slabLength

p0= geom.Pos3d(-halfWidth, halfHeight, 0.0)
p1= geom.Pos3d(-halfWidth, -halfHeight, 0.0)
p2= geom.Pos3d(halfWidth, -halfHeight, 0.0)
p3= geom.Pos3d(halfWidth, halfHeight, 0.0)

## K-points.
kPoints= list()
for p in [p0, p1, p2, p3]:
    kPoints.append(modelSpace.newKPoint(p.x, p.y, p.z))

leftWallLine= modelSpace.newLine(kPoints[1],kPoints[0])
bottomSlabLine= modelSpace.newLine(kPoints[1],kPoints[2])
rightWallLine= modelSpace.newLine(kPoints[2],kPoints[3])
topSlabLine= modelSpace.newLine(kPoints[0],kPoints[3])

## Materials.
concrete= EHE_materials.HA35
steel= EHE_materials.B500S
### Walls section.
wallRCSection= def_simple_RC_section.RCRectangularSection(name='wallRCSection', width= 1.0, depth= wallThickness, concrType= concrete, reinfSteelType= steel)
wallXCSection= wallRCSection.defElasticShearSection2d(preprocessor)
slabRCSection= def_simple_RC_section.RCRectangularSection(name='slabRCSection', width= 1.0, depth= slabThickness, concrType= concrete, reinfSteelType= steel)
slabXCSection= slabRCSection.defElasticShearSection2d(preprocessor)

# Generate mesh.
## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
## Seed element.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(wallXCSection)
beam2d= modelSpace.newSeedElement("ElasticBeam2d")
beam2d.h= wallThickness
## Generate mesh.
for l in [leftWallLine, rightWallLine]:
    l.nDiv= 10
    l.genMesh(xc.meshDir.I)
modelSpace.setDefaultMaterial(slabXCSection)
beam2d= modelSpace.newSeedElement("ElasticBeam2d")
beam2d.h= slabThickness

## Generate mesh.
for l in [bottomSlabLine, topSlabLine]:
    l.nDiv= 10
    l.genMesh(xc.meshDir.I)

# Constraints.
for pt in [kPoints[1], kPoints[2]]:
    n= pt.getNode()
    modelSpace.fixNode('00F', n.tag)

# Define sets.
xcTotalSet= modelSpace.getTotalSet()
concreteSet= modelSpace.defSet('concreteSet')
for l in xcTotalSet.lines:
    concreteSet.lines.append(l)  
concreteSet.fillDownwards()

### Define loads.
gravityVector= xc.Vector([0.0, g])
### Loads cases.
selfWeightCode= 'G'
loadCaseManager= lcm.LoadCaseManager(modelSpace.preprocessor)
loadCaseManager.defineSimpleLoadCases([selfWeightCode]) 
#### Self weight.
cLC= loadCaseManager.setCurrentLoadCase(selfWeightCode)
cLC.description= 'Self weight.'
modelSpace.createSelfWeightLoad(concreteSet, gravityVector)

# Check self weight.
modelSpace.addLoadCaseToDomain('G')
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
R= geom.Vector3d(0,0,0)
for n in xcTotalSet.nodes:
    R+= n.getReactionForce3d
selfWeightRefValue= boxVolume*2.5e3*g
ratio0= abs(R[1]-selfWeightRefValue)/selfWeightRefValue
ratio1= abs(R[0])

'''
print(R[1]/1e3, selfWeightRefValue/1e3, ratio0)
print((R[1]-selfWeightRefValue)/0.4/2.5e3/g)
print(R[0], ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-12 and abs(ratio1)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic output
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayLoads()
# oh.displayReactions()
