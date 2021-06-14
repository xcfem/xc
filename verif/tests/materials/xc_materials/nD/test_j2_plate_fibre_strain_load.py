# -*- coding: utf-8 -*-
from __future__ import print_function
''' Home made test
    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1.0 # Size of element edge (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
h= 2e-2
A= h*h # bar area expressed in square meters
I= (h)**4/12 # Cross section moment of inertia (m4)
AT= 10.0 # Temperature increment (Celsius degrees)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 # First node number.
nod1= nodes.newNodeXYZ(0.0,0.0,0.0)
nod2= nodes.newNodeXYZ(L,0.0,0.0)
nod3= nodes.newNodeXYZ(L,h,0.0)
nod4= nodes.newNodeXYZ(0,h,0.0)


# Materials definition
steel= typical_materials.defJ2PlateFibre(preprocessor=preprocessor, name='steel', E= E, nu=0.3, fy=250e6,alpha=1e-6,rho= 0.0)
memb1= typical_materials.defMembranePlateFiberSection(preprocessor,name='memb1',h= h ,nDMaterial= steel)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elements.defaultTag= 1
elem1= elements.newElement("ShellMITC4",xc.ID([nod1.tag,nod2.tag,nod3.tag,nod4.tag]))


# Constraints
constraints= preprocessor.getBoundaryCondHandler

spc= constraints.newSPConstraint(nod1.tag,0,0.0)
spc= constraints.newSPConstraint(nod2.tag,0,0.0)
spc= constraints.newSPConstraint(nod3.tag,0,0.0)
spc= constraints.newSPConstraint(nod4.tag,0,0.0)
spc= constraints.newSPConstraint(nod1.tag,2,0.0)
spc= constraints.newSPConstraint(nod2.tag,2,0.0)
spc= constraints.newSPConstraint(nod3.tag,2,0.0)
spc= constraints.newSPConstraint(nod4.tag,2,0.0)
spc= constraints.newSPConstraint(nod1.tag,1,0.0)
spc= constraints.newSPConstraint(nod2.tag,1,0.0)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID([elem1.tag])
eleLoad.setStrainComp(0,0,alpha*AT) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,0,alpha*AT)
eleLoad.setStrainComp(2,0,alpha*AT)
eleLoad.setStrainComp(3,0,alpha*AT)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

elem1.getResistingForce()
n1Medio= elem1.getMeanInternalForce("n1")
n2Medio= elem1.getMeanInternalForce("n2")
N1= (n1Medio*h)
N2= n2Medio

N1ref= (-E*A*alpha*AT)
ratio1= ((N1-N1ref)/N1ref)
ratio2= (N2)

''' 
print("N1ref= ",N1ref)
print("N1= ",N1)
print("ratio1= ",ratio1)
print("N2= ",N2)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<2e-7) & (abs(ratio2)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
