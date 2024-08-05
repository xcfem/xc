# -*- coding: utf-8 -*-
'''Check surface loads on brick element face.'''

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

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d", 25000,0.25,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Define mesh.
## Nodes.
x0= 0.0; x1= 3.0; x2= 6.0
z0= 0.0; z1= 3.0; z2= 6.0
yBottom= 0.0; yTop= 3.0
area= 6*6
### Bottom
nod1= nodes.newNodeXYZ(x0, yBottom, z2)
nod2= nodes.newNodeXYZ(x1, yBottom, z2)
nod3= nodes.newNodeXYZ(x2, yBottom, z2)
nod4= nodes.newNodeXYZ(x0, yBottom, z1)
nod5= nodes.newNodeXYZ(x1, yBottom, z1)
nod6= nodes.newNodeXYZ(x2, yBottom, z1)
nod7= nodes.newNodeXYZ(x0, yBottom, z0)
nod8= nodes.newNodeXYZ(x1, yBottom, z0)
nod9= nodes.newNodeXYZ(x2, yBottom, z0)
### Top
nod10= nodes.newNodeXYZ(x0, yTop, z2)
nod11= nodes.newNodeXYZ(x1, yTop, z2)
nod12= nodes.newNodeXYZ(x2, yTop, z2)
nod13= nodes.newNodeXYZ(x0, yTop, z1)
nod14= nodes.newNodeXYZ(x1, yTop, z1)
nod15= nodes.newNodeXYZ(x2, yTop, z1)
nod16= nodes.newNodeXYZ(x0, yTop, z0)
nod17= nodes.newNodeXYZ(x1, yTop, z0)
nod18= nodes.newNodeXYZ(x2, yTop, z0)

## Constraints
for n in [nod1, nod2, nod3, nod4, nod5, nod6, nod7, nod8, nod9]:
    n.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

## Pressure
pressure= -0.1

## Brick elements
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick1= elements.newElement("Brick",xc.ID([nod1.tag, nod2.tag, nod5.tag, nod4.tag, nod10.tag, nod11.tag, nod14.tag, nod13.tag]))
brick2= elements.newElement("Brick",xc.ID([nod2.tag, nod3.tag, nod6.tag, nod5.tag, nod11.tag, nod12.tag, nod15.tag, nod14.tag]))
brick3= elements.newElement("Brick",xc.ID([nod4.tag, nod5.tag, nod8.tag, nod7.tag, nod13.tag, nod14.tag, nod17.tag, nod16.tag]))
brick4= elements.newElement("Brick",xc.ID([nod5.tag, nod6.tag, nod9.tag, nod8.tag, nod14.tag, nod15.tag, nod18.tag, nod17.tag]))

# Surface load elements.
surfaceLoad5= elements.newElement("BrickSurfaceLoad",xc.ID([nod10.tag, nod11.tag, nod14.tag, nod13.tag]))
surfaceLoad6= elements.newElement("BrickSurfaceLoad",xc.ID([nod11.tag, nod12.tag, nod15.tag, nod14.tag]))
surfaceLoad7= elements.newElement("BrickSurfaceLoad",xc.ID([nod13.tag, nod14.tag, nod17.tag, nod16.tag]))
surfaceLoad8= elements.newElement("BrickSurfaceLoad",xc.ID([nod14.tag, nod15.tag, nod18.tag, nod17.tag]))

# Load pattern.
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("surface_load")
eleLoad.elementTags= xc.ID([surfaceLoad5.tag, surfaceLoad6.tag, surfaceLoad7.tag, surfaceLoad8.tag])
eleLoad.pressure= pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.
modelSpace.addLoadCaseToDomain(lp0.name) # Add the load case to the domain.

# Compute solution.
result= modelSpace.analyze(calculateNodalReactions= True)

# Compute reactions.
Ry= 0
for n in [nod1, nod2, nod3, nod4, nod5, nod6, nod7, nod8, nod9]:
    Ry+= n.getReaction[1]

# Compute force.
Fy= pressure*area
ratio1= abs(Fy+Ry)/Fy

'''
print('Ry= ', Ry)
print('Fy= ', Fy)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# # oh.displayLocalAxes()
# # oh.displayReactions()
