# -*- coding: utf-8 -*-
'''Earth pressure on a buried box (culvert or underpass structure) according to
the publication:

"Guia de cimentaciones en  obras de carretera". 
   ISBN 8449808626, 9788449808623
   https://books.google.es/books/about/Gu%C3%ADa_de_cimentaciones_en_obras_de_carre.html?hl=de&id=a0eoygAACAAJ&redir_esc=y
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from actions import load_cases as lcm
from scipy.constants import g
from actions.earth_pressure import earth_pressure as ep
from actions.earth_pressure import marston_effect as me

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
topSlabZ= halfHeight
bottomSlabZ= -halfHeight
## Box volume (per unit length).
boxVolume= 2*wallThickness*boxHeight+2*slabThickness*slabLength

p0= geom.Pos3d(-halfWidth, halfHeight, 0.0)
p1= geom.Pos3d(-halfWidth, -halfHeight, 0.0)
p2= geom.Pos3d(halfWidth, -halfHeight, 0.0)
p3= geom.Pos3d(halfWidth, halfHeight, 0.0)

## K-points.
kPoints= list()
for p in [p0, p1, p2, p3]:
    kPoints.append(modelSpace.newKPointFromPos3d(p))

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
    
## Define loads.
### Loads cases.
loadCaseManager= lcm.LoadCaseManager(modelSpace.preprocessor)
loadCaseNames= ['E']
loadCaseManager.defineSimpleLoadCases(loadCaseNames) 
    
### Define earth pressure load.
earthCover= 16.48
zGround= topSlabZ+earthCover
rhoSoil= 2e3
gammaSoil= rhoSoil*g
gammaWater= 1e3*g
phiSoil= math.radians(30)
tanDelta= math.tan(phiSoil) # consider delta==Phi 
KSoil= 0.5
embankmentSoil= ep.EarthPressureModel(zGround= zGround, zBottomSoils=[bottomSlabZ], KSoils=[KSoil], gammaSoils=[gammaSoil], zWater=-1e3, gammaWater=gammaWater)
## Vertical pressure on top slab.
pressureOnTopSlab= embankmentSoil.getVerticalPressure(topSlabZ)
### Vertical pressure increment due to marston effect.
D= earthCover
H= boxHeight+2*slabThickness
B= boxWidth+2*wallThickness
marstonPressureOnTopSlab= me.marston_vertical_pressure_increment(gammaSoil= gammaSoil, D= D, H=H, B= B, Er=1.0, Et= 10.0)
pressureOnTopSlab+= marstonPressureOnTopSlab
pressureOnTopSlabFactor= slabLength/topSlabLine.getLength()
pressureOnTopSlab*=pressureOnTopSlabFactor
loadVector= pressureOnTopSlab*xc.Vector([0,-1])
cLC= loadCaseManager.setCurrentLoadCase('E')
cLC.description= 'Empuje de tierras.'
for e in topSlabLine.elements:
    e.vector2dUniformLoadGlobal(loadVector)
## Pressure on walls.
halfVF= 0.0
for e in leftWallLine.elements:
    z= e.getPosCentroid(True).y
    pressure= embankmentSoil.getPressure(z)
    vf= -pressure*tanDelta # vertical friction.
    halfVF+= vf*e.getLength(True)
    loadVector= xc.Vector([pressure,vf])
    e.vector2dUniformLoadGlobal(loadVector)
for e in rightWallLine.elements:
    z= e.getPosCentroid(True).y
    pressure= -embankmentSoil.getPressure(z)
    vf= pressure*tanDelta # vertical friction.
    loadVector= xc.Vector([pressure,vf])
    e.vector2dUniformLoadGlobal(loadVector)

# Check earth pressure.
modelSpace.addLoadCaseToDomain('E')
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")
R= geom.Vector3d(0,0,0)
xcTotalSet= modelSpace.getTotalSet()
for n in xcTotalSet.nodes:
    R+= n.getReactionForce3d
marstonIncrement= 6.3047759356376645e3
verticalFriction= 2*250.96247716896972e3
earthPressureRefValue= 2.0e3*g*(earthCover)+marstonIncrement
earthWeight= earthPressureRefValue*slabLength+verticalFriction
ratio0= abs(R[1]-earthWeight)/earthWeight
ratio1= abs(R[0])

'''
print(ratio0)
print(ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-12 and abs(ratio1)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

 
# # Graphic output
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayLoads()
# oh.displayReactions()
