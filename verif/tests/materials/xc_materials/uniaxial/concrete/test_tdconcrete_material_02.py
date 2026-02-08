# -*- coding: utf-8 -*-
''' Trivial test of TDConcrete material constitutive model.  

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
 
Ec = 25*GPa # concrete modulus of elasticity
fc = -28*MPa # concrete compressive strength (compression is negative)
ft = 3*MPa # concrete tensile strength (tension is positive)
beta = 0.4 # Recommended value for the tension softening parameter (tension softening exponent).
tDry = 14 # days
tcast = 0 # analysis time corresponding to concrete casting (in days; minimum value 2.0)

# Creep and shrinkage parameters.
epsshu = -600e-6 # Ultimate shrinkage strain
psish = 75.4218 # Based on section dimensions
Tcr = 28 # creep model age (in days)
phiu = 3.0 # ultimate creep coefficient as per ACI 209R-92
psicr1 = 1.0 # Recommended value
psicr2 = 75.4218 # fitting parameter of the creep time evolution function as per ACI 209R-92. Based on section dimensions
csParameters= typical_materials.def_creep_and_shrinkage_parameters(tcr= Tcr, epsshu= epsshu, epssha= psish, epscru= phiu, epscra= psicr1, epscrd= psicr2)

## Concrete able to creep.
tdConcrete= typical_materials.defTDConcrete(preprocessor= preprocessor, name= 'tdConcrete',fpc= fc,ft= ft, Ec= Ec, beta= beta, age= tDry, tcast= tcast, csParameters= csParameters)

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
P= 1000*kN # axial load.

## Define time series.
ts= modelSpace.newTimeSeries(name= "ts", tsType= "constant_ts")
lp0= modelSpace.newLoadPattern(name= 'lp0')
modelSpace.setCurrentLoadPattern(lp0.name)
lp0.newNodalLoad(n2.tag, xc.Vector([-P, 0, 0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Define recorders.
ti= list()
concreteStresses= list()
steelStresses= list()
recorder= feProblem.getDomain.newRecorder("element_prop_recorder",None)
recorder.setElements(xc.ID([zl.tag]))
recorder.callbackRecord= "ti.append(self.getDomain.getTimeTracker.getCurrentTime); concreteStresses.append(concreteFiber.getStress()); steelStresses.append(steelFiber.getStress())"
recorder.callbackRestart= "print(\"Restart method called.\")"
# reactions= list()
# recReaction= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
# recReaction.setNodes(xc.ID([n1.tag]))
# recReaction.callbackRecord= "reactions.append(self.getReaction[0]);"
# recReaction.callbackSetup= "self.getDomain.calculateNodalReactions(True,1e-6)"

modelSpace.setCurrentTime(Tcr)

solProc= predefined_solutions.PlainNewtonRaphson(feProblem, printFlag= 0)
solProc.setup()
# Set the load control integrator with dt=0 so that the domain time doesn’t advance.
solProc.integrator.dLambda1= 0.0  
result= solProc.analysis.analyze(1)


dt = 10 # days
solProc.integrator.dLambda1= dt # set new increment for the integrator.
solProc.integrator.setNumIncr(10) # IMPORTANT! otherwise it got stuck.

modelSpace.setCreepOn() # Turn creep on
#modelSpace.setCreepDt(10) # set time increment for creep.

t = 0
while t < 10000:
    ok = solProc.analysis.analyze(1)
    t+= dt
errorDt= abs(solProc.integrator.dLambda1-dt)/dt # Make sure there is no modification of dLambda1
    
concreteForces= list()
steelForces= list()
errorForces= 0.0
avgSteelStress= 0.0
avgConcreteStress= 0.0
for sgc, sgs in zip(concreteStresses, steelStresses):
    avgConcreteStress+= sgc
    avgSteelStress+=sgs
    concreteForces.append(sgc*Ac)
    steelForces.append(sgs*As)
    force= sgc*Ac+sgs*As
    errorForces+=(force+P)**2
errorForces= math.sqrt(errorForces)
avgConcreteStress/=len(concreteStresses)
avgSteelStress/=len(steelStresses)

lastConcreteStress= concreteStresses[-1]
lastSteelStress= steelStresses[-1]

avgConcreteStressRef= 0.005974877488588469
ratio1= abs(avgConcreteStress+avgConcreteStressRef)/avgConcreteStressRef
avgSteelStressRef= 0.31414889483994685
ratio2= abs(avgSteelStress+avgSteelStressRef)/avgSteelStressRef

'''
print('time: ', ti)
print('concrete stresses: ', concreteStresses)
print('steel stresses: ', steelStresses)
# print('Reactions= ', reactions)
print(errorDt)
print(errorForces)
print('average concrete stress: ', avgConcreteStress, avgConcreteStressRef, ratio1)
print('average steel stress: ', avgSteelStress, avgSteelStressRef, ratio2)
print('time: ', modelSpace.getCurrentTime(), 'days')
print('last concrete stress: ', lastConcreteStress*1e3, 'MPa')
print('last steel stress: ', lastSteelStress*1e3, 'MPa')
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9) & (abs(ratio2)<1e-9) & (errorForces<1e-6) & (errorDt<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# #   FIGURES & REPORTS
# ## Scale stresses
# cStress= list()
# sStress= list()
# for cS, sS in zip(concreteStresses, steelStresses):
#     cStress.append(cS*1e3) # GPa -> MPa.
#     sStress.append(sS*1e3) # GPa -> MPa.
# import matplotlib.pyplot as plt
# fig = plt.figure()
# ax1 = fig.add_subplot(2, 1, 1)
# color= 'tab:blue'
# ax1.plot(ti,cStress, label= 'concrete stress', color= color)
# ax1.set_xlabel('time (days)')
# ax1.set_ylabel('concrete stress', color=color)
# ax1.set_xscale('log')
# ax1.legend()
# ax1.grid()

# ax2= ax1.twinx() # instantiate a second axes that shares the same x-axis
# color= 'tab:red'
# ax2.plot(ti,sStress, label= 'steel stress', color= color)
# ax2.legend()

# fig.tight_layout()  # otherwise the right y-label is slightly clipped
# plt.show()

