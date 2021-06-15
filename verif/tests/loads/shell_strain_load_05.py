# -*- coding: utf-8 -*-
''' home made test
    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''
from __future__ import print_function
from __future__ import division

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
nod1= nodes.newNodeXYZ(0.0,0.0,0.0)
nod2= nodes.newNodeXYZ(L,0.0,0.0)
nod3= nodes.newNodeXYZ(L,h,0.0)
nod4= nodes.newNodeXYZ(0,h,0.0)

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,0.3,0.0,h)

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
factor= 0.85
lp0= modelSpace.newLoadPattern(name= '0')
lp0.gammaF= factor
eleLoad= lp0.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID([elem1.tag])
eleLoad.setStrainComp(0,0,alpha*AT/2.0) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,0,alpha*AT/2.0)
eleLoad.setStrainComp(2,0,alpha*AT/2.0)
eleLoad.setStrainComp(3,0,alpha*AT/2.0)

lp1= modelSpace.newLoadPattern(name= '1')
lp1.gammaF= factor
eleLoad= lp1.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID([elem1.tag])
eleLoad.setStrainComp(0,0,alpha*AT/2.0) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,0,alpha*AT/2.0)
eleLoad.setStrainComp(2,0,alpha*AT/2.0)
eleLoad.setStrainComp(3,0,alpha*AT/2.0)

# We add the load cases to domain.
modelSpace.addLoadCaseToDomain(lp0.name)
modelSpace.addLoadCaseToDomain(lp1.name)

analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

# Material in the first Gauss point
mat= elem1.getPhysicalProperties.getVectorMaterials[0]
initialStrain= mat.initialSectionDeformation[0]
refInitialStrain= factor*alpha*AT
ratio0= abs(initialStrain-refInitialStrain)/refInitialStrain
strain= mat.getGeneralizedStrain()[0]
ratio1= abs(strain+initialStrain)/refInitialStrain

elem1.getResistingForce()
avgN1= elem1.getMeanInternalForce("n1")
avgN2= elem1.getMeanInternalForce("n2")
N1= (avgN1*h)
N2= avgN2

N= (-A*E*refInitialStrain)
ratio2= abs((N1-N)/N)
ratio3= abs(N2)

# Remove the load case from domain.
modelSpace.removeLoadCaseFromDomain(lp0.name)
result= analysis.analyze(1)
initialStrain2= mat.initialSectionDeformation[0]
refInitialStrain2= factor*alpha*AT/2.0
ratio4= abs(initialStrain2-refInitialStrain2)/refInitialStrain2
elem1.getResistingForce()
avgN1= elem1.getMeanInternalForce("n1")
avgN2= elem1.getMeanInternalForce("n2")
N1= (avgN1*h)
N2= avgN2
N= (-A*E*refInitialStrain2)
ratio5= abs((N1-N)/N)
ratio6= abs(N2)

''' 
print('initial strain: ', initialStrain)
print('reference initial strain: ', refInitialStrain)
print("ratio0= ",ratio0)
print('strain: ', strain)
print("ratio1= ",ratio1)
print("N= ",N)
print("N1= ",N1)
print("N2= ",N2)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print('initial strain 2: ', initialStrain2)
print('ratio4: ', ratio4)
print('ratio5: ', ratio5)
print('ratio6: ', ratio6)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0<1e-12) & (ratio1<1e-10) & (ratio2<1e-7) & (ratio3<1e-10) & (abs(ratio4)<1e-7) &(ratio5<1e-7)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
