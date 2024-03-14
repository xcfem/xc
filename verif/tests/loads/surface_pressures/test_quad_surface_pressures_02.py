# -*- coding: utf-8 -*-
''' Check surface loads on plane elements.

Test based on equilibrium equations. This test has been created to debug an
error in the computation of the quad surface load resisting force.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
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
L= 2.0
x0= 0.0; x1= L
y0= 0.0; y1= L
area= L
### Bottom
nod1= nodes.newNodeXY(x0,y0)
nod2= nodes.newNodeXY(x1,y0)
nod3= nodes.newNodeXY(x1,y1)
nod4= nodes.newNodeXY(x0,y1)

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
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("surface_load")
eleLoad.elementTags= xc.ID([surfaceLoad.tag])
eleLoad.pressure= pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.

# Compute solution for load 0.
modelSpace.addLoadCaseToDomain(lp0.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
Ry0= 0
for n in [nod1, nod2]:
    Ry0+= n.getReaction[1]

# Compute force.
Fy= pressure*area

ratio1= abs(Ry0+Fy)/Fy

'''
print('Ry0= ', Ry0)
print('Fy= ', Fy)
print('ratio1= ', ratio1)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
