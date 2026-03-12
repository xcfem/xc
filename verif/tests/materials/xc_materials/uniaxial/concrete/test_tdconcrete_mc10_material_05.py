# -*- coding: utf-8 -*-
''' Check tension behaviour of TDConcreteMC10 material constitutive model.

Based on the example: https://portwooddigital.com/2023/05/28/minimal-creep-and-shrinkage-example/
'''

from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Units: kN, mm
kN = 1
mm = 1
GPa = kN/mm**2
MPa = 0.001*GPa

# Define materials.

## Steel.
Es = 200*GPa
elast= typical_materials.defElasticMaterial(preprocessor, "elast",Es)

## Concrete.
Ec = 25*GPa # concrete modulus of elasticity at loading age.
Ecm= 30.303*GPa # 28-day modulus of elasticity
fcm = -28*MPa # mean 28-day cylinder compressive strength (compression is negative) at loading age.
ft = 3*MPa # concrete tensile strength (tension is positive) at loading age.
beta = 0.4 # Recommended value for the tension softening parameter (tension softening exponent).
tDry = 14 # days
tcast = 0 # analysis time corresponding to concrete casting (in days; minimum value 2.0)

# Shrinkage
epsba= -0.000034 # ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010. Row 21 column F in the TDCI_input spreadsheet.
epsbb= 1.000 # fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017. Row 22 column F in the TDCI_input spreadsheet.
epsda= -0.000853 # product of εcds,0 and βRH, as per Model Code 2010. Row 23 column F in the TDCI_input spreadsheet.
epsdb= 787.50 # fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017. Row 24 column F in the TDCI_input spreadsheet.

# Creep
phiba= 0.1747 # parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010. Row 20 column M in the TDCI_input spreadsheet.
phibb= 1.000 # fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017. Row 21 column M in the TDCI_input spreadsheet.
phida= 3.651 # product of βdc(fcm) and β(RH), as per Model Code 2010. Row 22 column M in the TDCI_input spreadsheet.
phidb= 504.5 # fitting constant within the drying creep time evolution function as per Model Code 2010. Row 23 column M in the TDCI_input spreadsheet.

# Cement type
cem= 1.0000 # coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.


## Concrete able to creep.
tdConcrete= typical_materials.defTDConcreteMC10(preprocessor= preprocessor, name= 'tdConcrete', fcm= fcm, ft= ft, Ec= Ec, Ecm= Ecm, beta= beta, age= tDry, epsba= epsba, epsbb= epsbb, epsda= epsda, epsdb= epsdb, phiba= phiba, phibb= phibb, phida= phida, phidb= phidb, tcast= 0.0, cem= cem)


b = 300*mm
h = 300*mm
As = 1500*mm**2
Ag = b*h
Ac = Ag-As

# Define mesh
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes) # Problem space.

## Define nodes.
n1= modelSpace.newNode(0, 0)
n2= modelSpace.newNode(0, 0)

## Define constraints.
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF00(n2.tag)

## Define fiber section.
twoFibersSection= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","twoFibersSection")
steelFiber= twoFibersSection.addFiber(elast.name, As, xc.Vector([0,0]))
concreteFiber= twoFibersSection.addFiber(tdConcrete.name, Ac, xc.Vector([0,0]))


## Create element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= twoFibersSection.name
elements.dimElem= 1 # Dimension of element space
zl= elements.newElement("ZeroLengthSection",xc.ID([n1.tag, n2.tag]))
concreteFiber= zl.getMaterial().getFibers().findFiber(concreteFiber.tag)
steelFiber= zl.getMaterial().getFibers().findFiber(steelFiber.tag)

# Define loads.
P0= 0.5*Ac*fcm # axial load.

## Define load pattern.
ts= modelSpace.newTimeSeries(name= "ts", tsType= "constant_ts")
lp0= modelSpace.newLoadPattern(name= 'lp0')
modelSpace.setCurrentLoadPattern(lp0.name)
lp0.newNodalLoad(n2.tag, xc.Vector([-P0, 0, 0])) # Compression load.
modelSpace.addLoadCaseToDomain(lp0.name)

# Compute solution.
# solProc= predefined_solutions.PlainNewtonRaphson(feProblem, maxNumIter= 150, convergenceTestTol= 1e-4*kN, printFlag= 0)
solProc= predefined_solutions.PlainStaticModifiedNewton(feProblem, convergenceTestTol= 1e-5*kN, maxNumIter= 150, printFlag= 0)
solProc.setup()
# Set the load control integrator with dt=0 so that the domain time doesn’t advance.
solProc.integrator.dLambda1= 0.0  
result= solProc.analysis.analyze(1)

# Define tension load.
P1= 1.5*Ac*ft-P0 # axial load.

## Define load pattern.
lp1= modelSpace.newLoadPattern(name= 'lp1')
modelSpace.setCurrentLoadPattern(lp1.name)
lp1.newNodalLoad(n2.tag, xc.Vector([P1, 0, 0])) # Tension load.
modelSpace.addLoadCaseToDomain(lp1.name)

for i in range(0, 10):
    factor= float((i+1)/10.0)
    lp1.gammaF= factor # trick to get a pseudo time-history
    result= solProc.analysis.analyze(1)

sigma_c= concreteFiber.getStress()
epsilon_c= concreteFiber.getStrain()
sigma_s= steelFiber.getStress()
epsilon_s= steelFiber.getStrain()
deltaP= P1-P0
ratio= (As*sigma_s-deltaP)/P0

# print('deltaP= ', deltaP)
# print('sigma_c= ', sigma_c/MPa, ' MPa')
# print('epsilon_c= ', epsilon_c*1e3)
# print('sigma_s= ', sigma_s/MPa, ' MPa')
# print('epsilon_s= ', epsilon_s*1e3)
# print('ratio= ', ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
