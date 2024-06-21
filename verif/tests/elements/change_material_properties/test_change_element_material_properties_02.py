# -*- coding: utf-8 -*-
'''Change the properties of the material of a ElasticBeam2d element.'''

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
b= 0.05 # Beam cross-section width. 
h= 0.05 # Beam cross-section depth.
A= b*h # Beam cross-section area.
E= 2.0e9 # Young modulus of the material.
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu)) # Shear modulus.
I= 1/12.0*b*h**3 # Inertia.
sry= 5/6 # Shear coefficient.
Ay= A/sry
# Load
F= 10e3

# Change factor
changeFactor= 0.5

import xc
from model import predefined_spaces
from materials import typical_materials
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Define mesh.
## Define nodes
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(1,0)

## Define materials.
mat= typical_materials.defElasticShearSection2d(preprocessor, "scc",A,E,G,I,alpha= Ay/A)

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

## Define element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= mat.name
elements.defaultTransformation= lin.name
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
beam2d.h= h

## Constraints
modelSpace.fixNode("000", n1.tag) # no displacement 
modelSpace.fixNode("F0F", n2.tag) # only Ux and rotation allowed.

## Loads.
lp0= modelSpace.newLoadPattern(name= '0')
nLoad= lp0.newNodalLoad(n2.tag, xc.Vector([F, 0.0, 0.0]))
### We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Compute solution. 
modelSpace.analyze(1)

# Get x displacement.
uX0= n2.getDisp[0]
uX0Ref= F/E/A
ratio0= abs(uX0-uX0Ref)/uX0Ref


# Access the materials on each integration point
for mat in beam2d.physicalProperties.getVectorMaterials:
    sectionProperties= mat.sectionProperties
    sectionProperties.E= changeFactor*E # Change material parameters.
    sectionProperties.G= 0.9*G

newE= beam2d.physicalProperties.getVectorMaterials[0].sectionProperties.E
new_nu= beam2d.physicalProperties.getVectorMaterials[0].sectionProperties.G

# Compute new solution. 
modelSpace.analyze(1)

# Get x displacement.
uX1= n2.getDisp[0]
uX1Ref= uX0Ref/changeFactor
ratio1= abs(uX1-uX1Ref)/uX1Ref

ratio2= (newE-changeFactor*E)/E
ratio3= (new_nu-0.9*G)/G

'''
print(uX0, uX0Ref, ratio0)
print(uX1, uX1Ref, ratio1)
print('old Young modulus: ', E, ' ksi')
print('old poisson ratio: ', nu)
print('new Young modulus: ', newE, ' ksi')
print('new poisson ratio: ', new_nu)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-6) and (abs(ratio1)<1e-6)  and (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

