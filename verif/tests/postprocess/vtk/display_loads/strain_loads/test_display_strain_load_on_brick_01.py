# -*- coding: utf-8 -*-
'''Check strain load representation on brick elements.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Material definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,0.0)
# Mesh
## Nodes.
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
n0= nodes.newNodeXYZ(0,0,0)
n1= nodes.newNodeXYZ(1,0,0)
n2= nodes.newNodeXYZ(1,1,0)
n3= nodes.newNodeXYZ(0,1,0)
n4= nodes.newNodeXYZ(0,0,1)
n5= nodes.newNodeXYZ(1,0,1)
n6= nodes.newNodeXYZ(1,1,1)
n7= nodes.newNodeXYZ(0,1,1)

## Elements.
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name
brick= elements.newElement("Brick",xc.ID([n0.tag, n1.tag, n2.tag, n3.tag, n4.tag, n5.tag, n6.tag, n7.tag]))

for n in [n0, n1, n2, n3]:
    modelSpace.fixNode('000', n.tag)

# Load case definition.
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 10.0 # Temperature increment (Celsius degrees)
strain= alpha*AT
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("brick_strain_load")
eleLoad.elementTags= xc.ID([brick.tag])
for gaussPoint in range(0, 8):
    for gdl in range(0, 3):
        eleLoad.setStrainComp(gaussPoint,gdl,strain) #(id of Gauss point, id of component, value)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

    
# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

## Uncomment to display the mesh
# oh.displayFEMesh()
# for strainLabel in ['epsilon_xx', 'epsilon_yy', 'epsilon_zz', 'epsilon_xy', 'epsilon_xz', 'epsilon_yz']:
#     oh.displayLoads(elLoadComp= strainLabel)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(elLoadComp= 'epsilon_xx', fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)
