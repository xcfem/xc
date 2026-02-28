# -*- coding: utf-8 -*-
''' Trivial test of TDConcreteMC10 material constitutive model. 

This test uses the concrete defined in he example A3 of the book:
Dujmovic, Darko., Androic, Boris., Lukacevic, Ivan. Composite Structures According to Eurocode 4: Worked Examples. Germany: Wiley, 2015.
ISBN 978-3-433-03107-0 
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
from materials.ec2 import EC2_materials
from materials.mc10 import MC10_td_concrete
from model import predefined_spaces
from solution import predefined_solutions

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define materials.

## Steel.
Es = 200*1e9
elast= typical_materials.defElasticMaterial(preprocessor, "elast",Es)

## Concrete.
concrete= EC2_materials.EC2Concrete("C25/30", -25e6, 1.5)
concrete.cemType= 'N'# class N cement
beta = 0.4 # Recommended value for the tension softening parameter (tension softening exponent).
ts= 0 # time at start of drying
age= 7 # concrete age at first loading.
RH= 70 # ambient relative humidity(%)

# Concrete geometry
hc= 0.12 # deck slab thickness.
Ag= 2.5*hc # Concrete slab area per m width.
ba= 0.18 # Steel beam flange width.
u= 2*2.5 - ba # Perimeter of the concrete part which is exposed to drying, u
h0= 2*Ag/u  # notional size of the member h0.

tdConcrete= MC10_td_concrete.get_TDConcrete_mc10(preprocessor= preprocessor, name= 'tdConcrete', concrete= concrete, cement= concrete.cemType, h0= h0, T= 21, RH= RH, beta= beta, ts= ts, age= age)

As = 2262e-6
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
P= 500688 # axial load.

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

Tcr = 28 # creep model age (in days)
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

avgConcreteStressRef= 0.6509115783229376e6
ratio1= abs(avgConcreteStress+avgConcreteStressRef)/avgConcreteStressRef
avgSteelStressRef= 135.6705961508777e6
ratio2= abs(avgSteelStress+avgSteelStressRef)/avgSteelStressRef

'''
# print('time: ', ti)
# print('concrete stresses: ', concreteStresses)
# print('steel stresses: ', steelStresses)
# print('Reactions= ', reactions)
print('errorDt= ', errorDt)
print('errorForces= ', errorForces)
print('average concrete stress: ', avgConcreteStress/1e6, avgConcreteStressRef/1e6, ratio1)
print('average steel stress: ', avgSteelStress/1e6, avgSteelStressRef/1e6, ratio2)
print('time: ', modelSpace.getCurrentTime(), 'days')
print('last concrete stress: ', lastConcreteStress*1e-6, 'MPa')
print('last steel stress: ', lastSteelStress*1e-6, 'MPa')
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-7) & (abs(ratio2)<1e-7) & (errorForces<1e-6) & (errorDt<1e-12):
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

