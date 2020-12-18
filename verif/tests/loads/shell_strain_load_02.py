# -*- coding: utf-8 -*-
from __future__ import print_function
''' Home made test based on symmetry and equilibrium considerations.
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
nu= 0.3
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 10.0 # Temperature increment (Celsius degrees)
h= 0.2
A= L*h

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(0.0,0.0,0.0)
nod2= nodes.newNodeXYZ(L,0.0,0.0)
nod3= nodes.newNodeXYZ(L,L,0.0)
nod4= nodes.newNodeXYZ(0,L,0.0)
nodeTags= [nod1.tag,nod2.tag,nod3.tag,nod4.tag]

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,h)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem1= elements.newElement("ShellNLDKGQ",xc.ID(nodeTags))

# Constraints
constraints= preprocessor.getBoundaryCondHandler

for tag in nodeTags:
    for dof in [0,1,2]:
        spc= constraints.newSPConstraint(tag,dof,0.0)

# Load case definition.
factor= 0.85
lp0= modelSpace.newLoadPattern(name= '0')
lp0.gammaF= factor
eleLoad= lp0.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID([elem1.tag])
for gp in [0,1,2,3]:
    for compo in [0,1]:
        eleLoad.setStrainComp(gp,compo,alpha*AT) #(id of Gauss point, id of component, value)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

modelSpace.analysis= predefined_solutions.penalty_newton_raphson(feProblem)
result= modelSpace.analyze(calculateNodalReactions= True)

# Material in the first Gauss point
mat= elem1.getPhysicalProperties.getVectorMaterials[0]
initialStrain= mat.initialSectionDeformation[0]
refInitialStrain= factor*alpha*AT
ratio0= abs(initialStrain-refInitialStrain)/refInitialStrain
strain= mat.getGeneralizedStrain()[0]
ratio1= abs(strain+initialStrain)/refInitialStrain

elem1.getResistingForce()
n1Medio= elem1.getMeanInternalForce("n1")
n2Medio= elem1.getMeanInternalForce("n2")
axil1= (n1Medio*L)
axil2= (n2Medio*L)

N= (-A*E*refInitialStrain)/(1-nu)
ratio2= abs((axil1-N)/N)
ratio3= abs((axil2-N)/N)

''' 
print('initial strain: ', initialStrain)
print('reference initial strain: ', refInitialStrain)
print("ratio0= ",ratio0)
print('strain: ', strain)
print("ratio1= ",ratio1)
print("N= ",N)
print("axil1= ",axil1)
print("ratio2= ",ratio2)
print("axil2= ",axil2)
print("ratio3= ",ratio3)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0<1e-12) & (ratio1<1e-4) & (ratio2<1e-4) & (ratio3<1e-4)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

