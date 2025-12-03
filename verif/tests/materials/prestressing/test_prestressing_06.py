# -*- coding: utf-8 -*-
''' Prestressing of a concrete truss using Dywidag bars. Home made test.'''
from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from scipy.constants import g
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.eota import eta_05_0123
from solution import predefined_solutions

concrete= EC2_materials.C40
l= 20 # Bar length
alpha= 1.0e-5 # Thermal expansion coefficient of concrete and steel 1/ºC

# Create FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Define mesh.
## Nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0)

## Define elements.
### Materials.
xcConcrete= concrete.defElasticMaterial(preprocessor)
dywidagBar= eta_05_0123.bar_36E
xcSteel= dywidagBar.getInitStressMaterial(preprocessor= preprocessor)

### Concrete truss.
modelSpace.setElementDimension(2)
modelSpace.setDefaultMaterial(xcConcrete)
concreteTruss= modelSpace.newElement("Truss", [n1.tag,n2.tag])
concreteTruss.sectionArea= 0.5*0.5
modelSpace.setDefaultMaterial(xcSteel)
steelBarTruss= modelSpace.newElement("Truss", [n1.tag,n2.tag])
steelBarTruss.sectionArea= dywidagBar.getNominalArea()

# Constraints
modelSpace.fixNode00(n1.tag)
modelSpace.fixNodeF0(n2.tag)


# Solution procedure
tol= 1e-6
solProc= predefined_solutions.PenaltyModifiedNewtonUMF(feProblem, printFlag= 0,convergenceTestTol= tol, convTestType= 'norm_unbalance_conv_test')

# Prestressing load.
prestressingLoad= 70e3*g # 70 t
prestressingStress= prestressingLoad/dywidagBar.getNominalArea()
lpP= modelSpace.newLoadPattern(name= 'P')
eleLoad= lpP.newElementalLoad("truss_prestress_load")
eleLoad.elementTags= xc.ID([steelBarTruss.tag])
eleLoad.sigma1= prestressingStress
eleLoad.sigma2= prestressingStress
modelSpace.addLoadCaseToDomain(lpP.name)

## Solve for prestressing.
analOk= solProc.solve(True)
if(analOk!=0):
    lmsg.error("Can't solve")

R01= n1.getReaction[0]
ratio01= abs(R01)
R02= n2.getReaction[0]
ratio02= abs(R02)

barInitStress0= steelBarTruss.getMaterial().initialStress
barStress0= steelBarTruss.getMaterial().getStress()
barStrain0= steelBarTruss.getMaterial().getStrain()
barStiffness0= steelBarTruss.getMaterial().getTangent()
barN= steelBarTruss.getN()
concreteStress0= concreteTruss.getMaterial().getStress()
concreteStrain0= concreteTruss.getMaterial().getStrain()
uX0= n2.getDisp[0]

# Shrinkage.
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("truss_strain_load")
eleLoad.elementTags= xc.ID([concreteTruss.tag])
initialShrinkage=  -6.733333333333333e-05
eleLoad.eps1= initialShrinkage
eleLoad.eps2= initialShrinkage
modelSpace.addLoadCaseToDomain(lp0.name)

## Solve for shrinking.
analOk= solProc.solve(True)
if(analOk!=0):
    lmsg.error("Can't solve")

R11= n1.getReaction[0]
ratio11= abs(R11)
R12= n2.getReaction[0]
ratio12= abs(R12)
barInitStress1= steelBarTruss.getMaterial().initialStress
barStress1= steelBarTruss.getMaterial().getStress()
concreteStress1= concreteTruss.getMaterial().getStress()
concreteStrain1= concreteTruss.getMaterial().getStrain()
uX1= n2.getDisp[0]

# Thermal expansion.
lp1= modelSpace.newLoadPattern(name= '1')
eleLoad= lp1.newElementalLoad("truss_strain_load")
eleLoad.elementTags= xc.ID([concreteTruss.tag, steelBarTruss.tag])
AT= 50.0
thermalStrain= alpha*AT
eleLoad.eps1= thermalStrain
eleLoad.eps2= thermalStrain
modelSpace.addLoadCaseToDomain(lp1.name)

## Solve for thermal expansion.
analOk= solProc.solve(True)

R21= n1.getReaction[0]
ratio21= abs(R21)
R22= n2.getReaction[0]
ratio22= abs(R22)
barInitStress2= steelBarTruss.getMaterial().initialStress
barStress2= steelBarTruss.getMaterial().getStress()
concreteStress2= concreteTruss.getMaterial().getStress()
uX2= n2.getDisp[0]
thermalElongation= thermalStrain*l
uX2Ref= thermalElongation+uX1-n1.getDisp[0]
ratio23= abs(uX2-uX2Ref)

# Thermal contraction.
modelSpace.removeLoadCaseFromDomain(lp1.name)

## Solve for thermal contraction.
analOk= solProc.solve(True)

R31= n1.getReaction[0]
ratio31= abs(R31)
R32= n2.getReaction[0]
ratio32= abs(R32)
barInitStress3= steelBarTruss.getMaterial().initialStress
barStress3= steelBarTruss.getMaterial().getStress()
concreteStress3= concreteTruss.getMaterial().getStress()
uX3= n2.getDisp[0]
uX3Ref= -alpha*AT*l+uX2
ratio33= abs(uX3-uX3Ref)

'''
print('Prestressing')
print("R01= ",R01)
print("ratio01= ",ratio01)
print("R02= ",R02)
print("ratio02= ",ratio02)
print("uX0= ",uX0)
print('concrete stress: ', concreteStress0/1e6)
print(barInitStress0/1e6, barStress0/1e6,'\n')
print('Shrinkage')
print("R11= ",R11)
print("ratio11= ",ratio11)
print("R12= ",R12)
print("ratio12= ",ratio12)
print("uX1= ",uX1)
print('concrete stress: ', concreteStress1/1e6)
print(barInitStress1/1e6, barStress1/1e6,'\n')
print('Thermal expansion')
print("R21= ",R21)
print("ratio21= ",ratio21)
print("R22= ",R22)
print("ratio22= ",ratio22)
print("uX2= ",uX2)
print("uX2Ref= ",uX2Ref)
print("ratio23= ",ratio23)
print("thermal strain= ", thermalStrain)
print("thermal elongation= ", thermalElongation)
print('concrete stress: ', concreteStress2/1e6)
print(barInitStress2/1e6, barStress2/1e6,'\n')
print('Thermal contraction')
print("R31= ",R31)
print("ratio31= ",ratio31)
print("R32= ",R32)
print("ratio32= ",ratio32)
print("uX3= ",uX3)
print("uX3Ref= ",uX3Ref)
print("ratio33= ",ratio33)
print('concrete stress: ', concreteStress3/1e6)
print(barInitStress3/1e6, barStress3/1e6,'\n')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio01<1e-6 and ratio02<1e-6 and ratio11<1e-6 and ratio12<1e-6 and ratio21<1e-6 and ratio22<1e-6 and ratio23<1e-6 and ratio31<1e-6 and ratio32<1e-6 and ratio33<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')




