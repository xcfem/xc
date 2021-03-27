# -*- coding: utf-8 -*-
''' Home made test. Thermal strains on Steel02.'''
from __future__ import print_function

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
from misc_utils import log_messages as lmsg


L= 1.0 # Bar length (m)
fy= 12600*9.81/1e-4 # Yield stress of the steel.
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
AT= 50 # Temperature increment (Celsius degrees)
initialStress= E*alpha*AT

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nod1= nodes.newNodeXY(0.0,0.0)
nod2= nodes.newNodeXY(L,0.0)

# Materials definition
steel= typical_materials.defSteel02(preprocessor, "steel",E,fy,0.001, initialStress)
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= steel.name
elements.dimElem= 2 # Dimension of element space
truss= elements.newElement("Truss",xc.ID([nod1.tag,nod2.tag]))
truss.sectionArea= A

    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc1= constraints.newSPConstraint(nod1.tag,0,0.0)
spc2= constraints.newSPConstraint(nod1.tag,1,0.0)
spc3= constraints.newSPConstraint(nod2.tag,0,0.0)
spc4= constraints.newSPConstraint(nod2.tag,1,0.0)

# Solution procedure.
tol= 1e-8
solProc= predefined_solutions.PenaltyNewtonLineSearchUMF(feProblem, printFlag= 0, convergenceTestTol= tol, maxNumIter= 6, convTestType= 'norm_unbalance_conv_test')

# Solve for prestressing.
analOk= solProc.solve(True)
if(analOk!=0):
    lmsg.error('Failed to solve for prestressing.')
    quit()
truss.getResistingForce()
N0= truss.getN()
Nref0= initialStress*A
ratio01= abs(N0-Nref0)
if(abs(Nref0)>1e-6):
    ratio01/=Nref0
initStrain0= truss.getMaterial().initialStrain
ratio02= abs(initStrain0)
initStress0= truss.getMaterial().initialStress
strain0= truss.getMaterial().getStrain()
strain0Ref= initStress0/E
ratio03= abs(strain0-strain0Ref)/strain0Ref
stress0= truss.getMaterial().getStress()
ratio04= abs(stress0-initialStress)/initialStress

# Load case definition.
lpTE= modelSpace.newLoadPattern(name= 'TE')
eleLoadTE= lpTE.newElementalLoad("truss_strain_load")
eleLoadTE.elementTags= xc.ID([truss.tag])
initialStrainTE= alpha*AT
eleLoadTE.eps1= initialStrainTE
eleLoadTE.eps2= initialStrainTE
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lpTE.name)
analOk= solProc.solve(True)
if(analOk!=0):
    lmsg.error('Failed to solve for thermal expansion.')
    quit()
truss.getResistingForce()
Nref1= initialStress*A-E*A*initialStrainTE
ratio11= abs(truss.getN()-Nref1)
if(abs(Nref1)>1e-6):
    ratio11/=Nref1
initStrain1= truss.getMaterial().initialStrain
ratio12= abs(initStrain1-initialStrainTE)/initialStrainTE
initStress1= truss.getMaterial().initialStress
strain1= truss.getMaterial().getStrain()
strain1Ref= (initialStress/E-initialStrainTE)
ratio13= abs(strain1-strain1Ref)
if(abs(strain1Ref>1e-6)):
    ratio13/= strain1Ref
stress1= truss.getMaterial().getStress()
stress1Ref= initialStress-initialStrainTE*E 
ratio14= abs(stress1-stress1Ref)
if(abs(stress1Ref)>1e-6):
    ratio14/=stress1Ref

# Thermal contraction.
lpTC= modelSpace.newLoadPattern(name= 'TC')
eleLoadTC= lpTC.newElementalLoad("truss_strain_load")
eleLoadTC.elementTags= xc.ID([truss.tag])
initialStrainTC= -alpha*AT
eleLoadTC.eps1= initialStrainTC
eleLoadTC.eps2= initialStrainTC
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lpTC.name)
#result= modelSpace.analyze(calculateNodalReactions= False)
analOk= solProc.solve(True)
if(analOk!=0):
    lmsg.error('Failed to solve for thermal contraction.')
    quit()
truss.getResistingForce()
N2= truss.getN()
Nref2= initialStress*A
ratio21= abs(N2-Nref2)
if(abs(Nref2)>1e-6):
    ratio21/=Nref2
initStrain2= truss.getMaterial().initialStrain
ratio22= abs(initStrain2)
initStress2= truss.getMaterial().initialStress
strain2= truss.getMaterial().getStrain()
strain2Ref= initialStress/E
ratio23= abs(strain1-strain1Ref)
if(abs(strain2Ref>1e-6)):
    ratio23/= strain2Ref
stress2= truss.getMaterial().getStress()
stress2Ref= initialStress
ratio24= abs(stress2-stress2Ref)
if(abs(stress2Ref)>1e-6):
    ratio24/= stress2Ref

'''
print("initial stress: ",initialStress)
print("fy= ",fy/1e6)
print("N0= ",N0/1e3)
print("Nref0= ",Nref0/1e3)
print("ratio01= ",ratio01)
print("initial strain 0: ", initStrain0)
print("ratio02= ",ratio02)
print("initial stress 0: ", initStress0)
print("strain 0: ", strain0)
print("ratio03= ",ratio03)
print("stress 0: ", stress0)
print("ratio04= ",ratio04,'\n')

print("ratio11= ",ratio11)
print("initial strain 1: ", initStrain1)
print("ratio12= ",ratio12)
print("initial stress 1: ", initStress1)
print("strain 1: ", strain1)
print("ratio13= ",ratio13)
print("stress 1: ", stress1)
print("ratio14= ",ratio14,'\n')

print("N2= ",N2/1e3)
print("Nref2= ",Nref2/1e3)
print("ratio21= ",ratio21)
print("initial strain 2: ", initStrain2)
print("ratio22= ",ratio22)
print("initial stress 2: ", initStress2)
print("strain 2: ", strain2)
print("ratio23= ",ratio23)
print("stress 2: ", stress2)
print("stress2Ref= ", stress2Ref)
print("ratio24= ",ratio24)
'''

import os
fname= os.path.basename(__file__)
if(abs(ratio01)<1e-5 and (abs(ratio02)<1e-5) and abs(ratio03)<1e-5 and (abs(ratio04)<1e-5) and abs(ratio11)<1e-5 and (abs(ratio12)<1e-5) and (abs(ratio13)<1e-5) and (abs(ratio14)<1e-5) and (abs(ratio21)<1e-5) and (abs(ratio22)<1e-5) and (abs(ratio23)<1e-5) and (abs(ratio24)<1e-5)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
