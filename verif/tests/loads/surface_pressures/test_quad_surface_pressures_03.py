# -*- coding: utf-8 -*-
''' Check surface loads on plane elements.

'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
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
modelSpace.fixNode("00", nod1.tag)
modelSpace.fixNode("F0", nod2.tag)

## Pressure
pressure= -0.1e3

## Quad elements
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast2d.name
quad= elements.newElement("FourNodeQuad",xc.ID([nod1.tag, nod2.tag, nod3.tag, nod4.tag]))

# Surface load elements.
quadSurfaceLoad1= elements.newElement("QuadSurfaceLoad",xc.ID([nod1.tag, nod2.tag]))
quadSurfaceLoad2= elements.newElement("QuadSurfaceLoad",xc.ID([nod2.tag, nod3.tag]))
quadSurfaceLoad3= elements.newElement("QuadSurfaceLoad",xc.ID([nod3.tag, nod4.tag]))
quadSurfaceLoad4= elements.newElement("QuadSurfaceLoad",xc.ID([nod4.tag, nod1.tag]))

# Load pattern.
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("surface_load")
eleLoad.dim= 2 # 2D space.
eleLoad.pressure= pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.
eleLoad.elementTags= xc.ID([quadSurfaceLoad1.tag])

lp1=modelSpace.newLoadPattern(name= '1')
eleLoad= lp1.newElementalLoad("surface_load")
eleLoad.dim= 2 # 2D space.
eleLoad.pressure= pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.
eleLoad.elementTags= xc.ID([quadSurfaceLoad2.tag])

lp2=modelSpace.newLoadPattern(name= '2')
eleLoad= lp2.newElementalLoad("surface_load")
eleLoad.dim= 2 # 2D space.
eleLoad.pressure= pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.
eleLoad.elementTags= xc.ID([quadSurfaceLoad3.tag])

lp3=modelSpace.newLoadPattern(name= '3')
eleLoad= lp3.newElementalLoad("surface_load")
eleLoad.dim= 2 # 2D space.
eleLoad.pressure= pressure # applied pressure loading normal to the surface, outward is positive, inward is negative.
eleLoad.elementTags= xc.ID([quadSurfaceLoad4.tag])

modelSpace.addLoadCaseToDomain(lp0.name)
modelSpace.addLoadCaseToDomain(lp1.name)
modelSpace.addLoadCaseToDomain(lp2.name)
modelSpace.addLoadCaseToDomain(lp3.name)
modelSpace.analyze(calculateNodalReactions= True)
Rx= nod1.getReaction[0]
Ry= nod1.getReaction[1]+nod2.getReaction[1]
err= math.sqrt(Rx**2+Ry**2)

# print('Rx= ', Rx, 'Ry= ', Ry, ' err= ', err)

import os
fname= os.path.basename(__file__)
if err<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayLoads()
# #oh.displayReactions()

