# -*- coding: utf-8 -*-
''' Trivial test concering reinforced concrete rectangular section.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

import math
from materials.sections.fiber_section import def_simple_RC_section
from materials.ec2 import EC2_materials
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from rough_calculations import ng_simple_beam as sb

# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

# Geometry of the reinforcement.
spacing= 0.15 # spacing of reinforcement.
nBarsA= 10 # number of bars.
cover= 0.035 # concrete cover.
lateralCover= cover # concrete cover for the bars at the extremities of the row.
width= nBarsA*spacing+2.0*lateralCover

## First row
barDiameter= 25e-3 # Diameter of the reinforcement bar.
barAreaA= math.pi*(barDiameter/2.0)**2 # Area of the reinforcement bar.
### Reinforcement row.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, areaRebar= barAreaA, rebarsSpacing= spacing, width= width, nominalCover= cover, nominalLatCover= lateralCover)
areaA= rowA.getAs()

## Second row
barAreaB= math.pi*(barDiameter/2.0)**2 # Area of the reinforcement bar.
### Reinforcement row.
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, areaRebar= barAreaB, rebarsSpacing= spacing, width= width-spacing, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)
areaB= rowB.getAs()
area= areaA+areaB

# Define reinforcement layers.
reinfLayers= def_simple_RC_section.LongReinfLayers([rowA, rowB])

# Define reinforced concrete rectangular section.
## Materials.
concrete= EC2_materials.C20
steel= EC2_materials.S500C
## Geometry
b= width
h= 0.20
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='RC section', width= b, depth= h, concrType= concrete, reinfSteelType= steel)
rcSection.positvRebarRows= reinfLayers # Reinforcement on the positive side.

# Define finite element problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
elasticProblem= False
## Materials.
if(elasticProblem):
    eSection= rcSection.defElasticShearSection2d(preprocessor) # Elastic cross-section.
    beamSection= eSection
    elementType= "ElasticBeam2d"
else:
    rcSection.defRCSection2d(preprocessor,matDiagType= 'k') # Fiber-section material.
    beamSection= rcSection
    elementType= "ForceBeamColumn2d"

## Create mesh.
### Create nodes.
span= 5
numDiv= 10
beamNodes= list()
for i in range(0,numDiv+1):
    x= i/numDiv*span
    beamNodes.append(nodes.newNodeXY(x,0.0))

### Create elements.
#### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin")
elemHandler= preprocessor.getElementHandler
elemHandler.defaultTransformation= lin.name # Coordinate transformation for the new elements
elemHandler.defaultMaterial= beamSection.name

beamElements= list()
# n0= beamNodes[0]
# for n1 in beamNodes[1:]:
#     beamElements.append(elemHandler.newElement(elementType,xc.ID([n0.tag,n1.tag])))
#     n0= n1
reversedNodes= list(reversed(beamNodes))
n0= reversedNodes[0]
for n1 in reversedNodes[1:]:
    beamElements.append(elemHandler.newElement(elementType,xc.ID([n0.tag,n1.tag])))
    n0= n1

## Constraints
nA= beamNodes[0]
nB= beamNodes[-1]
nC= beamNodes[5]
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode00F(nA.tag) # First node pinned.
modelSpace.fixNodeF0F(nB.tag) # Last node pinned.

## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
q= -5e3
#q= 5e3
loadVector= xc.Vector([0.0, q])
for e in beamElements:
    e.vector2dUniformLoadGlobal(loadVector)
### We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem, maxNumIter= 20)
result= analysis.analyze(1)
if(result!=0):
    print('Can\'t solve.')
    exit(1)

# If the program reaches this point the reinforcement is placed in the right
# side of the section (try to inverse the applied load and you will see that
# the solver crashes (there is no reinforcement to resist the inverse load).


# Get reactions.
nodes.calculateNodalReactions(True,1e-7) 
vDisp= xc.Vector([nC.getDisp[0],nC.getDisp[1]])
vReacA= xc.Vector([nA.getReaction[0],nA.getReaction[1]])
vReacB= xc.Vector([nB.getReaction[0],nB.getReaction[1]])

# Check results
halfSpan= span/2.0
ratio1= abs(nC.getCoo[0]-halfSpan)/(halfSpan)

print('element type= ', elementType)
print(nC.getCoo[0])
print('ratio1= ', ratio1)
print('vertical reactions: ', vReacA, vReacB)
print('deflection = ', vDisp[1]*1e3, 'mm')



#########################################################
# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

## Uncomment to display the mesh
#oh.displayFEMesh()

## Uncomment to display the element axes
oh.displayLocalAxes()

## Uncomment to display the loads
#oh.displayLoads()

## Uncomment to display the vertical displacement
oh.displayDispRot(itemToDisp='uY')
#oh.displayNodeValueDiagram(itemToDisp='uX')

## Uncomment to display the reactions
#oh.displayReactions()

## Uncomment to display the internal force
oh.displayIntForcDiag('Mz')
oh.displayIntForcDiag('Vy')

