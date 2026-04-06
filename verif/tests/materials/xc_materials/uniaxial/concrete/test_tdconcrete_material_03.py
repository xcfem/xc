# -*- coding: utf-8 -*-
''' Check tension behaviour of TDConcrete material constitutive model.  

Based on the example: https://portwooddigital.com/2023/05/28/minimal-creep-and-shrinkage-example/
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
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
 
Ec = 25*GPa # concrete modulus of elasticity
fc = -28*MPa # concrete compressive strength (compression is negative)
ft = 3*MPa # concrete tensile strength (tension is positive)
beta = 0.4 # Recommended value for the tension softening parameter (tension softening exponent).
tDry = 14 # days
tcast = 0 # analysis time corresponding to concrete casting (in days; minimum value 2.0)

# Creep and shrinkage parameters.
Tcr = 28 # creep model age (in days)
epsshu = -600e-6 # Ultimate shrinkage strain
psish = 75.4218 # Based on section dimensions
phiu = 3.0 # ultimate creep coefficient as per ACI 209R-92
psicr1 = 1.0 # Recommended value
psicr2 = 75.4218 # fitting parameter of the creep time evolution function as per ACI 209R-92. Based on section dimensions
csParameters= typical_materials.def_aci_creep_and_shrinkage_parameters(tcr= Tcr, epsshu= epsshu, epssha= psish, epscru= phiu, epscra= psicr1, epscrd= psicr2)

## Concrete able to creep (even if in this example creep will not be used).
tdConcrete= typical_materials.defTDConcrete(preprocessor= preprocessor, name= 'tdConcrete',fpc= fc,ft= ft, Ec= Ec, beta= beta, age= tDry, tcast= tcast, csParameters= csParameters)
## Linear elastic steel.
Es = 200*GPa
elast= typical_materials.defElasticMaterial(preprocessor, "elast",Es)

# Cross-section geometry
b = 300*mm
h = 300*mm
As = 1500*mm**2
Ag = b*h # Gross area.
Ac = Ag-As # Concrete area.

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

# Define compression load.
P0= 0.5*Ac*fc # axial load.

## Define load pattern.
ts= modelSpace.newTimeSeries(name= "ts", tsType= "constant_ts")
lp0= modelSpace.newLoadPattern(name= 'lp0')
modelSpace.setCurrentLoadPattern(lp0.name)
lp0.newNodalLoad(n2.tag, xc.Vector([-P0, 0, 0])) # Compression load.
modelSpace.addLoadCaseToDomain(lp0.name)

# Compute solution.
solProc= predefined_solutions.PlainNewtonRaphson(feProblem, maxNumIter= 150, convergenceTestTol= 1e-4*kN, printFlag= 0)
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
sigma_s= steelFiber.getStress()
deltaP= P1-P0
ratio= (As*sigma_s-deltaP)/P0

# print('deltaP= ', deltaP)
# print('sigma_c= ', sigma_c/MPa, ' MPa')
# print('sigma_s= ', sigma_s/MPa, ' MPa')
# print('ratio= ', ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
