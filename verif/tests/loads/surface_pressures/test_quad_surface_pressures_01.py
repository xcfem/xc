# -*- coding: utf-8 -*-
'''Test taken from example 5-001 of SAP2000 verification manual.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",25000,0.25,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Define mesh.
## Nodes.
x0= 0.0; x1= 1.0
y0= 0.0; y1= 1.0
area= 1.0
### Bottom
nod1= nodes.newNodeXY(0,0)
nod2= nodes.newNodeXY(1,0)
nod3= nodes.newNodeXY(1,1)
nod4= nodes.newNodeXY(0,1)

## Constraints
for n in [nod1, nod2]:
    n.fix(xc.ID([0,1]),xc.Vector([0,0]))

## Pressure
pressure= -0.1

## Quad elements
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast2d.name
quad= elements.newElement("FourNodeQuad",xc.ID([nod1.tag, nod2.tag, nod3.tag, nod4.tag]))

# Surface load elements.
surfaceLoad= elements.newElement("QuadSurfaceLoad",xc.ID([nod4.tag, nod3.tag]))

# Load pattern.
## First load pattern.
lpA= modelSpace.newLoadPattern(name= 'A')
eleLoad= lpA.newElementalLoad("surface_load")
eleLoad.elementTags= xc.ID([surfaceLoad.tag])
eleLoad.pressure= pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.
## Second load pattern.
lpB= modelSpace.newLoadPattern(name= 'B')
eleLoad= lpB.newElementalLoad("surface_load")
eleLoad.elementTags= xc.ID([surfaceLoad.tag])
eleLoad.pressure= -pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.
## First load combination.
combC= modelSpace.newLoadCombination("C","1.0*A+1.0*B")
## Secont load combination.
combD= modelSpace.newLoadCombination("D","1.0*A+1.0*A")


# Compute solution for load A.
modelSpace.addLoadCaseToDomain(lpA.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
RyA= 0
for n in [nod1, nod2]:
    RyA+= n.getReaction[1]

# Compute solution for load B.
modelSpace.resetLoadCase()
modelSpace.addLoadCaseToDomain(lpB.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
RyB= 0
for n in [nod1, nod2]:
    RyB+= n.getReaction[1]

# Compute solution for combination C.
modelSpace.resetLoadCase()
modelSpace.addLoadCaseToDomain(combC.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
RyC= 0
for n in [nod1, nod2]:
    RyC+= n.getReaction[1]

# Compute solution for combination D.
modelSpace.resetLoadCase()
modelSpace.addLoadCaseToDomain(combD.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
RyD= 0
for n in [nod1, nod2]:
    RyD+= n.getReaction[1]
    
# Compute force.
Fy= pressure*area

ratio1= abs(RyA+Fy)/Fy
ratio2= abs(RyB-Fy)/Fy
ratio3= abs(RyC)/Fy
ratio4= abs(RyD+2*Fy)/Fy

'''
print('RyA= ', RyA)
print('Fy= ', Fy)
print('ratio1= ', ratio1)
print('RyB= ', RyB)
print('ratio2= ', ratio2)
print('RyC= ', RyC)
print('ratio3= ', ratio3)
print('RyD= ', RyD)
print('ratio4= ', ratio4)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
