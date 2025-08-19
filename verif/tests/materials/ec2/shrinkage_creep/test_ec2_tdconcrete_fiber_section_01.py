# -*- coding: utf-8 -*-
''' Trivial test of TDConcreteMC10 material constitutive model. This test is 
    similar to the previous one, but the definition of concrete cross-section
    is simplified by using the def_simple_RC_section module.

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
from materials.ec2 import EC2_materials
from materials.mc10 import MC10_materials
from materials.sections.fiber_section import def_simple_RC_section
from model import predefined_spaces
from solution import predefined_solutions

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define materials.

## Steel.
reinfSteel= EC2_materials.S500B

## Concrete.
concrete= EC2_materials.EC2Concrete("C20/25", -20e6, 1.5)
beta = 0.4 # Recommended value for the tension softening parameter (tension softening exponent).
ts= 14 # time at start of drying
age= 7 # concrete age at first loading.

# Define the parameters needed to create a TDConcrete diagram.
concrete.defTDConcreteParameters(beta= beta, cement= '42.5R', h0= 0.15, T= 21, RH= 50, ts= ts, t0= age)


b= 0.3
h= 0.3
As = 1500e-6
rebarArea= (As/4)
rebarDiam= math.sqrt(4*rebarArea/math.pi)
Ag = b*h
Ac = Ag-As
nominalCover= 0.035

# Define reinforce concrete cross-section.
## Reinforcement.
rebarRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= b, nRebars= 2, nominalCover= nominalCover, nominalLatCover= nominalCover)
## Concrete section template.
rcSection= def_simple_RC_section.RCRectangularSection(name='test', width= b, depth= h, concrType= concrete, reinfSteelType= reinfSteel)
rcSection.positvRebarRows= def_simple_RC_section.LongReinfLayers([rebarRow])
rcSection.negatvRebarRows= def_simple_RC_section.LongReinfLayers([rebarRow])
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
rcSection.defRCSection2d(preprocessor, matDiagType= 'td')

## Create element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= rcSection.name
elements.dimElem= 1 # Dimension of element space
zl= elements.newElement("ZeroLengthSection",xc.ID([n1.tag, n2.tag]))

fiberSection= zl.getSection()
fibers= fiberSection.getFibers()
steelFibers= list()
concreteFibers= list()
for fiber in fibers:
    material= fiber.getMaterial()
    if(material.name=='dgES500B'):
        steelFibers.append(fiber)
    else:
        concreteFibers.append(fiber)

# Define loads.
P= 600*1e3 # axial load.

## Define time series.
ts= modelSpace.newTimeSeries(name= "ts", tsType= "constant_ts")
lp0= modelSpace.newLoadPattern(name= 'lp0')
modelSpace.setCurrentLoadPattern(lp0.name)
lp0.newNodalLoad(n2.tag, xc.Vector([-P, 0, 0]))
modelSpace.addLoadCaseToDomain(lp0.name)

Tcr = 28 # creep model age (in days)
modelSpace.setCurrentTime(Tcr)

solProc= predefined_solutions.PlainNewtonRaphson(feProblem, convergenceTestTol= 1e-5, printFlag= 0)
solProc.setup()
# Set the load control integrator with dt=0 so that the domain time doesn’t advance.
solProc.integrator.dLambda1= 0.0  
result= solProc.analysis.analyze(1)

# Compute initial stresses.
avgInitialConcreteStress= 0.0
for fiber in concreteFibers:
    avgInitialConcreteStress+= fiber.getStress()
avgInitialConcreteStress/=len(concreteFibers)
avgInitialSteelStress= 0.0
for fiber in steelFibers:
    avgInitialSteelStress+= fiber.getStress()
avgInitialSteelStress/=len(steelFibers)


dt = 100 # days
solProc.integrator.dLambda1= dt # set new increment for the integrator.
solProc.integrator.setNumIncr(10) # IMPORTANT! otherwise it got stuck.

modelSpace.setCreepOn() # Turn creep on

t = 0
while t < 10000:
    ok = solProc.analysis.analyze(1)
    t+= dt
errorDt= abs(solProc.integrator.dLambda1-dt)/dt # Make sure there is no modification of dLambda1

# Compute final stresses.
avgFinalConcreteStress= 0.0
for fiber in concreteFibers:
    avgFinalConcreteStress+= fiber.getStress()
avgFinalConcreteStress/=len(concreteFibers)
avgFinalSteelStress= 0.0
for fiber in steelFibers:
    avgFinalSteelStress+= fiber.getStress()
avgFinalSteelStress/=len(steelFibers)

avgFinalConcreteStressRef= 2.2216951865375707e6
ratio1= abs(avgFinalConcreteStress+avgFinalConcreteStressRef)/avgFinalConcreteStressRef
avgFinalSteelStressRef= 266.69828880774594e6
ratio2= abs(avgFinalSteelStress+avgFinalSteelStressRef)/avgFinalSteelStressRef

'''
print(fiberSection.getStressResultant())
print('errorDt= ', errorDt)
print('Initial concrete stress (average): ',  avgInitialConcreteStress/1e6, 'MPa')
print('Initial steel stress (average): ',  avgInitialSteelStress/1e6, 'MPa')
print('Final concrete stress (average): ',  avgFinalConcreteStress/1e6, 'MPa', ratio1)
print('Final steel stress (average): ',  avgFinalSteelStress/1e6, 'MPa', ratio2)
# rcSection.pdfReport()
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-7) & (abs(ratio2)<1e-7) & (errorDt<1e-12):
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

