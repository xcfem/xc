# -*- coding: utf-8 -*-
''' home made test
    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

L= 1.0 # Size of element edge (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
h= 2e-2
AT= 10.0 # Temperature increment (Celsius degrees)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nod1= nodes.newNodeXY(0.0,0.0)
nod2= nodes.newNodeXY(L,0.0)
nod3= nodes.newNodeXY(L,h)
nod4= nodes.newNodeXY(0,h)

# Materials definition
memb1= typical_materials.defElasticIsotropicPlaneStrain(preprocessor, name= "memb1", E= E, nu= 0.3, rho= 0.0)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem1= elements.newElement("FourNodeQuad",xc.ID([nod1.tag,nod2.tag,nod3.tag,nod4.tag]))


# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(nod1.tag,0,0.0)
spc= constraints.newSPConstraint(nod2.tag,0,0.0)
spc= constraints.newSPConstraint(nod3.tag,0,0.0)
spc= constraints.newSPConstraint(nod4.tag,0,0.0)
spc= constraints.newSPConstraint(nod1.tag,1,0.0)
spc= constraints.newSPConstraint(nod2.tag,1,0.0)

# Load case definition.
factor= 0.85
lp0= modelSpace.newLoadPattern(name= '0')
lp0.gammaF= factor
eleLoad= lp0.newElementalLoad("quad_strain_load")
eleLoad.elementTags= xc.ID([elem1.tag])
eleLoad.setStrainComp(0,0,alpha*AT) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,0,alpha*AT)
eleLoad.setStrainComp(2,0,alpha*AT)
eleLoad.setStrainComp(3,0,alpha*AT)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

# Material in the first Gauss point
mat= elem1.physicalProperties.getVectorMaterials[0]
initialStrain= mat.initialStrain[0]
refInitialStrain= factor*alpha*AT
ratio0= abs(initialStrain-refInitialStrain)/refInitialStrain
strain= mat.getStrain()[0]
ratio1= abs(strain+initialStrain)/refInitialStrain

elem1.getResistingForce()
avgN1= elem1.getMeanInternalForce("n1")
avgN2= elem1.getMeanInternalForce("n2")
N1= avgN1
N2= avgN2

N= L*(-E*refInitialStrain)
ratio2= abs((N1-N)/N) # Mesh is too coarse so this result is quite poor.
ratio3= abs(N2)

# Remove the load case from domain.
modelSpace.removeLoadCaseFromDomain(lp0.name)
result= analysis.analyze(1)
initialStrain2= mat.initialStrain[0]
elem1.getResistingForce()
ratio4= abs(elem1.getMeanInternalForce("n1"))

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
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0<1e-12) & (ratio1<1e-5) & (ratio2<0.15) & (ratio3<1e-8) & (abs(initialStrain2)<1e-7) &(ratio4<1e-8)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# modelSpace.addLoadCaseToDomain(lp0.name)
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# for strainLabel in ['epsilon_xx', 'epsilon_yy', 'epsilon_xy']:
#     oh.displayLoads(elLoadComp= strainLabel)
