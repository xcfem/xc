# -*- coding: utf-8 -*-
'''Check surface loads on brick element face.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d", 25000,0.25,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Define mesh.
## Nodes.
area= 1.0
### Bottom.
nod1= nodes.newNodeXYZ(0,0,0)
nod2= nodes.newNodeXYZ(1,0,0)
nod3= nodes.newNodeXYZ(1,1,0)
nod4= nodes.newNodeXYZ(0,1,0)
### Top.
nod5= nodes.newNodeXYZ(0,0,1)
nod6= nodes.newNodeXYZ(1,0,1)
nod7= nodes.newNodeXYZ(1,1,1)
nod8= nodes.newNodeXYZ(0,1,1)

## Constraints
constrainedNodes= [nod1, nod2, nod3, nod4]
for n in constrainedNodes:
    n.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))


## Brick elements
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick= elements.newElement("Brick",xc.ID([nod1.tag, nod2.tag, nod3.tag, nod4.tag, nod5.tag, nod6.tag, nod7.tag, nod8.tag]))

# Surface load elements.
topNodes= [nod5.tag, nod6.tag, nod7.tag, nod8.tag]
brickSurfaceLoad= elements.newElement("BrickSurfaceLoad",xc.ID(topNodes))

# Load pattern.
## Pressure
pressure= -1e3
## First load pattern.
lp= modelSpace.newLoadPattern(name= 'lp')
eleLoad= lp.newElementalLoad("surface_load")
eleLoad.dim= 3 # 3D space.
eleLoad.pressure= pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.
eleLoad.elementTags= xc.ID([brickSurfaceLoad.tag])
    
# Compute solution.
modelSpace.addLoadCaseToDomain(lp.name) # Add the load case to the domain.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
Rx= .0
Ry= .0
Rz= .0
for n in constrainedNodes:
    Rx+= n.getReaction[0]
    Ry+= n.getReaction[1]
    Rz+= n.getReaction[2]
    
# Compute force.
Fz= pressure*area

err= math.sqrt(Rx**2+Ry**2+(Rz+Fz)**2)

'''
print('Rz= ', Rz)
print('Fz= ', Fz)
print('err= ', err)
'''

import os
fname= os.path.basename(__file__)
if (err<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# oh.displayLoads()
# # oh.displayLocalAxes()
# oh.displayReactions()
