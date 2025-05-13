# -*- coding: utf-8 -*-
'''Change the properties of the material of a ShellMITC4 element.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= modelSpace.newNodeXYZ(0,0,0)
n2= modelSpace.newNodeXYZ(1,0,0)
n3= modelSpace.newNodeXYZ(1,1,0)
n4= modelSpace.newNodeXYZ(0,1,0)

# Materials definition
E= 2.1e6 # Steel Young's modulus [kg/cm2].
nu= 0.3 # Poisson's ratio.
h= 0.1 # thickness.
dens= 1.33 # specific mass [kg/m2].
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat", E, nu, dens, h)

elements= preprocessor.getElementHandler
elements.defaultMaterial= shellMat.name
shell= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))

# Access the materials on each integration point
for mat in shell.physicalProperties.getVectorMaterials:
    mat.E= 2*E # Change material parameters.
    mat.nu= 0.9*nu
    
newE= shell.physicalProperties.getVectorMaterials[0].E
new_nu= shell.physicalProperties.getVectorMaterials[0].nu

ratio1= (newE-2*E)/E
ratio2= (new_nu-0.9*nu)/nu

'''
print('old Young modulus: ', oldE, ' ksi')
print('old poisson ratio: ', old_nu)
print('new Young modulus: ', newE, ' ksi')
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
