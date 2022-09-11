# -*- coding: utf-8 -*-
''' Example based on the post: Ordinary Eigenvalues of Prof. Michael Scott
https://portwooddigital.com/2020/11/13/ordinary-eigenvalues/
'''

from __future__ import print_function
from __future__ import division
import xc

from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

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
v11= n1.getEigenvector(1)[2]
ratio3= abs(v11+refValue)/refValue
v12= n2.getEigenvector(2)[2]
ratio4= abs(v12+refValue)/refValue
v21= n2.getEigenvector(1)[2]
ratio5= abs(v21-refValue)/refValue
v22= n2.getEigenvector(2)[2]
ratio6= abs(v22+refValue)/refValue


'''
print('computed eigenvalues: ', eig1, eig2)
print('expected eigenvalues: ', eig1Ref, eig2Ref)
print('         ratio: ', ratio1, ratio2)
print('Eigenvector 1')
print(v11,v21)
print(ratio3,ratio4)
print('Eigenvector 2')
print(v12,v22)
print(ratio5,ratio6)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-9) and (abs(ratio4)<1e-9) and (abs(ratio5)<1e-9) and (abs(ratio6)<1e-9):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
