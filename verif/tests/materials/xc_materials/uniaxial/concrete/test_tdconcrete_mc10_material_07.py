# -*- coding: utf-8 -*-
''' Check tension behaviour of TDConcreteMC10 material constitutive model when
    the shrinkage is constrained.

Based on the example: https://portwooddigital.com/2023/05/28/minimal-creep-and-shrinkage-example/
'''

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
from misc_utils import log_messages as lmsg

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Units: N, m
kN = 1e3
mm = 1e-3
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

## Model Code 2010 shrinkage and creep parameters.
mc10CreepShrinkageParameters= typical_materials.def_mc10_creep_and_shrinkage_parameters(epsba= epsba, epsbb= epsbb, epsda= epsda, epsdb= epsdb, phiba= phiba, phibb= phibb, phida= phida, phidb= phidb, cem= cem)

## Concrete able to creep.
tdConcrete= typical_materials.defTDConcreteMC10(preprocessor= preprocessor, name= 'tdConcrete', fcm= fcm, ft= ft, Ec= Ec, Ecm= Ecm, beta= beta, age= tDry, mc10CSParameters= mc10CreepShrinkageParameters, tcast= 0.0)


L= 10.0 # Length
side= 700*mm
b= side
h= side
As= 1500*mm**2
Ag= b*h
Ac= Ag-As

# Define mesh
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes) # Problem space.

## Define nodes.
n1= modelSpace.newNode(0, 0)
n2= modelSpace.newNode(L, 0)

## Define constraints.
modelSpace.fixNode('00', n1.tag)
modelSpace.fixNode('F0', n2.tag)

## Define fiber section.
twoFibersSection= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","twoFibersSection")
steelFiber= twoFibersSection.addFiber(elast.name, As, xc.Vector([0,0]))
concreteFiber= twoFibersSection.addFiber(tdConcrete.name, Ac, xc.Vector([0,0]))


## Create element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= twoFibersSection.name
elements.dimElem= 2 # Dimension of element space
truss= elements.newElement("TrussSection",xc.ID([n1.tag, n2.tag]))
concreteFiber= truss.getMaterial().getFibers().findFiber(concreteFiber.tag)
steelFiber= truss.getMaterial().getFibers().findFiber(steelFiber.tag)

Tcr = 28 # creep model age (in days)
modelSpace.setCurrentTime(Tcr)

solProc= predefined_solutions.PlainNewtonRaphson(feProblem, maxNumIter= 20, convergenceTestTol= 1e-6, printFlag= 0)
solProc.setup()
# Set the load control integrator with dt=0 so that the domain time doesn’t advance.
solProc.integrator.dLambda1= 0.0  
result= solProc.analysis.analyze(1)
if(result!=0):
    lmsg.error("Can't solve for the initial state.")
    exit(1)

dt = 10 # days
solProc.integrator.dLambda1= dt # set new increment for the integrator.
solProc.integrator.setNumIncr(dt) # IMPORTANT! otherwise it got stuck.

modelSpace.setCreepOn() # Turn creep on
#modelSpace.setCreepDt(10) # set time increment for creep.

t = 0
while t < 10000:
    ok = solProc.analysis.analyze(1)
    if(ok!=0):
        lmsg.error("Can't solve for time: "+str(t)+' days.')
        exit(1)
    t+= dt
errorDt= abs(solProc.integrator.dLambda1-dt)/dt # Make sure there is no modification of dLambda1

sigma_c= concreteFiber.getStress()
epsilon_c= concreteFiber.getStrain()
sigma_s= steelFiber.getStress()
epsilon_s= steelFiber.getStrain()
ratio0= abs(epsilon_c-epsilon_s)
ratio1= sigma_s*As+sigma_c*Ac
N= truss.getN()
ratio2= abs(N)
modelSpace.calculateNodalReactions()
Rx= n2.getReaction[0]
ratio3= abs(Rx)
Ux= n2.getDisp[0]

'''
print('sigma_c= ', sigma_c/MPa, ' MPa')
print('epsilon_c= ', epsilon_c*1e3)
print('sigma_s= ', sigma_s/MPa, ' MPa')
print('epsilon_s= ', epsilon_s*1e3)
print('ratio0= ', ratio0)
print('ratio1= ', ratio1)
print('N= ', N/1e3)
print('ratio2= ', ratio2)
print('Rx= ', Rx/1e3)
print('ratio3= ', ratio3)
print('Ux= ', Ux*1e3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-6) and (abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
