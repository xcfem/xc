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
# from postprocess import output_handler

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d", 25000,0.25,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Define mesh.
## Nodes.
x0= 0.0; x1= 1.0
z0= 0.0; z1= 1.0
y0= 0.0; y1= 1.0
area= 1.0
### Bottom
nod1= nodes.newNodeXYZ(0,0,0)
nod2= nodes.newNodeXYZ(1,0,0)
nod3= nodes.newNodeXYZ(1,1,0)
nod4= nodes.newNodeXYZ(0,1,0)
nod5= nodes.newNodeXYZ(0,0,1)
nod6= nodes.newNodeXYZ(1,0,1)
nod7= nodes.newNodeXYZ(1,1,1)
nod8= nodes.newNodeXYZ(0,1,1)

## Constraints
for n in [nod1, nod2, nod3, nod4]:
    n.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

## Pressure
pressure= -0.1

## Brick elements
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick= elements.newElement("Brick",xc.ID([nod1.tag, nod2.tag, nod3.tag, nod4.tag, nod5.tag, nod6.tag, nod7.tag, nod8.tag]))

# Surface load elements.
surfaceLoad= elements.newElement("BrickSurfaceLoad",xc.ID([nod5.tag, nod6.tag, nod7.tag, nod8.tag]))

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
RzA= 0
for n in [nod1, nod2, nod3, nod4]:
    RzA+= n.getReaction[2]

# Compute solution for load B.
modelSpace.resetLoadCase()
modelSpace.addLoadCaseToDomain(lpB.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
RzB= 0
for n in [nod1, nod2, nod3, nod4]:
    RzB+= n.getReaction[2]

# Compute solution for combination C.
modelSpace.resetLoadCase()
modelSpace.addLoadCaseToDomain(combC.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
RzC= 0
for n in [nod1, nod2, nod3, nod4]:
    RzC+= n.getReaction[2]

# Compute solution for combination D.
modelSpace.resetLoadCase()
modelSpace.addLoadCaseToDomain(combD.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
RzD= 0
for n in [nod1, nod2, nod3, nod4]:
    RzD+= n.getReaction[2]
    
# Compute force.
Fz= pressure*area

ratio1= abs(RzA+Fz)/Fz
ratio2= abs(RzB-Fz)/Fz
ratio3= abs(RzC)/Fz
ratio4= abs(RzD+2*Fz)/Fz

'''
print('RzA= ', RzA)
print('Fz= ', Fz)
print('ratio1= ', ratio1)
print('RzB= ', RzB)
print('ratio2= ', ratio2)
print('RzC= ', RzC)
print('ratio3= ', ratio3)
print('RzD= ', RzD)
print('ratio4= ', ratio4)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
