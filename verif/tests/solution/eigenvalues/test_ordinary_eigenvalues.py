# -*- coding: utf-8 -*-
''' Example based on the post: Ordinary Eigenvalues of Prof. Michael Scott
https://portwooddigital.com/2020/11/13/ordinary-eigenvalues/
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import math
import xc
import geom
from model import predefined_spaces
from materials import typical_materials

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

L = 120
E = 29000
A = 12
I = 800

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Mesh

## Nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(L,0)

## Elements.

### Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin")

### Material.
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E
sectionProperties.I= I
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)

### Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
beam2d01= elements.newElement("ElasticBeam2d",xc.ID([n1.tag, n2.tag]))

### Constraints
modelSpace.fixNode00F(n1.tag)
modelSpace.fixNode00F(n2.tag)

# Solution.
analOk= modelSpace.ordinaryEigenvalues(2)
eig1= modelSpace.analysis.getEigenvalue(1)
eig1Ref= 2*E*I/L
ratio1= abs(eig1-eig1Ref)/eig1Ref

eig2= modelSpace.analysis.getEigenvalue(2)
eig2Ref= 3*eig1Ref
ratio2= abs(eig2-eig2Ref)/eig2Ref

refValue= math.sqrt(2)/2.0

refVector1= geom.Vector2d(refValue, -refValue)

# Mode 1
v1= n1.getEigenvector(1)[2] # (mode)[component]
v2= n2.getEigenvector(1)[2] # (mode)[component]
vector1= geom.Vector2d(v1, v2)
ratio3= min((refVector1-vector1).getModulus(),(refVector1+vector1).getModulus())/refValue # sign doesn't count

#Mode 2
v1= n2.getEigenvector(2)[2] # (mode)[component]
v2= n2.getEigenvector(2)[2] # (mode)[component]
vector2= geom.Vector2d(v1, v2)
refVector2= geom.Vector2d(refValue, refValue)
ratio4= min((refVector2-vector2).getModulus(),(refVector2+vector2).getModulus())/refValue # sign doesn't count

'''
print('computed eigenvalues: ', eig1, eig2)
print('expected eigenvalues: ', eig1Ref, eig2Ref)
print('         ratio: ', ratio1, ratio2)
print('Eigenvector 1: ', vector1)
print('Reference eigenvector 1: ', refVector1)
print('ratio3= ', ratio3)
print('Eigenvector 2: ', vector2)
print('Reference eigenvector 2: ', refVector2)
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-9) and (abs(ratio4)<1e-9):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
