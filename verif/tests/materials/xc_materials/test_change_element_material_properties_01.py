# -*- coding: utf-8 -*-
'''Change the material of an element.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 6.0 # Beam length expressed in inches.
h= 0.8 # Beam cross-section depth expressed in inches.
t= 1 # Beam cross-section width expressed in inches. In the article t= 0.2 was written by mistake.
E= 30000 # Young modulus of the material expressed in ksi.
nu= 0.3 # Poisson's ratio.

import xc
from model import predefined_spaces
from materials import typical_materials
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Define mesh.
## Define nodes
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(1,0)
n3= nodes.newNodeXY(1,1)
n4= nodes.newNodeXY(0,1)

## Define materials.
mat= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "mat",E,nu,0.0)

## Define element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= mat.name
quad= elements.newElement("FourNodeQuad",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))
quad.thickness= t

# Access the materials on each integration point
for mat in quad.physicalProperties.getVectorMaterials:
    mat.E= 2*E # Change material parameters.
    mat.nu= 0.9*nu

newE= quad.physicalProperties.getVectorMaterials[0].E
new_nu= quad.physicalProperties.getVectorMaterials[0].nu

ratio1= (newE-2*E)/E
ratio2= (new_nu-0.9*nu)/nu

'''
print('old young modulus: ', oldE, ' ksi')
print('old poisson ratio: ', old_nu)
print('new young modulus: ', newE, ' ksi')
print('new poisson ratio: ', new_nu)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

