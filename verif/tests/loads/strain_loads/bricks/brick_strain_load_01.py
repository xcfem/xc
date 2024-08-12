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
# from postprocess import output_handler

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Material definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,0.0)
# Mesh
## Nodes.
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod9= nodes.newNodeXYZ(0,0,0)
nod10= nodes.newNodeXYZ(1,0,0)
nod11= nodes.newNodeXYZ(1,1,0)
nod12= nodes.newNodeXYZ(0,1,0)
nod13= nodes.newNodeXYZ(0,0,1)
nod14= nodes.newNodeXYZ(1,0,1)
nod15= nodes.newNodeXYZ(1,1,1)
nod16= nodes.newNodeXYZ(0,1,1)

## Elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick= elements.newElement("Brick",xc.ID([nod9.tag,nod10.tag,nod11.tag,nod12.tag,nod13.tag,nod14.tag,nod15.tag,nod16.tag]))

constraints= preprocessor.getBoundaryCondHandler
# Constrain the displacement of the nodes in the base.
nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod10.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod11.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod12.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

# Load case definition.
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 10.0 # Temperature increment (Celsius degrees)
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("brick_strain_load")
eleLoad.elementTags= xc.ID([brick.tag])
for gaussPoint in range(0, 8):
    for gdl in range(0, 3):
        eleLoad.setStrainComp(gaussPoint,gdl,alpha*AT) #(id of Gauss point, id of component, value)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

R9= nod9.getReaction
R10= nod10.getReaction
R11= nod11.getReaction
R12= nod12.getReaction


RA= R9+R12
RB= R10+R11
R= RA+RB

ratio1= R.Norm()
testOK= (ratio1<1e-12) and (RA.Norm()>0) and (RB.Norm()>0) 

''' 
print("R9= ",R9)
print("R12= ",R12)
print("RA= ", RA)
print("R10= ",R10)
print("R11= ",R11)
print("RA= ", RB)
print("R= ",R)
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

# ## Uncomment to display the mesh
# oh.displayFEMesh()
# oh.displayLoads()
