# -*- coding: utf-8 -*-
from __future__ import print_function
''' Home made test
    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials


L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
AT= 10 # Temperature increment (Celsius degrees)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nod1= nodes.newNodeXY(0.0,0.0)
nod2= nodes.newNodeXY(L,0.0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast.name
elements.dimElem= 2 # Dimension of element space
truss= elements.newElement("Truss",xc.ID([nod1.tag,nod2.tag]))
truss.sectionArea= A

    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc1= constraints.newSPConstraint(nod1.tag,0,0.0)
spc2= constraints.newSPConstraint(nod1.tag,1,0.0)
spc3= constraints.newSPConstraint(nod2.tag,0,0.0)
spc4= constraints.newSPConstraint(nod2.tag,1,0.0)
    
# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("truss_strain_load")
eleLoad.elementTags= xc.ID([truss.tag])
eleLoad.eps1= alpha*AT
eleLoad.eps2= alpha*AT
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)
result= modelSpace.analyze(calculateNodalReactions= False)

truss.getResistingForce()
N= (-E*A*alpha*AT)
ratio1= ((truss.getN()-N)/N)
initStrain1= truss.getMaterial().initialStrain
ratio2= abs(initStrain1-alpha*AT)/(alpha*AT)

# Remove load case from domain.
modelSpace.removeLoadCaseFromDomain(lp0.name)
result= modelSpace.analyze(calculateNodalReactions= False)
truss.getResistingForce()
ratio3= truss.getN()
initStrain3= truss.getMaterial().initialStrain
ratio4= abs(initStrain3)

'''
print("ratio1= ",ratio1)
print("initial strain 1: ", initStrain1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("initial strain 3: ", initStrain3)
print("ratio4= ",ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-5 and (abs(ratio2)<1e-5) and (abs(ratio3)<1e-5) and (abs(ratio4)<1e-5)):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
