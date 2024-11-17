# -*- coding: utf-8 -*-
''' Pressure-displacement diagram verification test. Check equilibrium
when "softing" the element at the left side due to a minor depth caused
by an excavation.'''


from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc
from geotechnics import earth_pressure
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions

# Define soil model.
phi= math.pi/6.0
rankineSoil= earth_pressure.RankineSoil(phi, rho= 2000)

# Define depth and tributary area corresponding to the spring simulating
# the soil reaction.
depth= 3.0 # 3 m
tributaryArea= 1.5 # 1.5 m2
Kh= 30e6 # subgrade horizontal soil reaction.

# FE problem definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
## Problem type
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Nonlinear spring material
sg_v= rankineSoil.getVerticalStressAtDepth(z= depth)
nlSpringMaterial= rankineSoil.defHorizontalSubgradeReactionNlMaterial(preprocessor, name= 'nlSpringMaterial', sg_v= sg_v, tributaryArea= tributaryArea, Kh= Kh)

# Spring opposed to soil movement to simulate the earth-retaining structure.
k= typical_materials.defElasticMaterial(preprocessor, "k",E= 1e9)

maxForce= rankineSoil.getPassivePressure(sg_v= sg_v)*tributaryArea
xMax= maxForce/k.E


## Define mesh.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0,0)

## Constraints
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF00(n2.tag)

## Define element.
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Dimension of element space

### Left element.
elements.defaultMaterial= nlSpringMaterial.name
zlLeft= elements.newElement("ZeroLength",xc.ID([n2.tag,n1.tag]))
soilMaterialLeft= zlLeft.getMaterials()[0]
zlLeft.setupVectors(xc.Vector([-1,0,0]),xc.Vector([0,-1,0]))

### Right element.
elements.defaultMaterial= nlSpringMaterial.name
zlRight= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))
soilMaterialRight= zlRight.getMaterials()[0]
zlRight.setupVectors(xc.Vector([1,0,0]),xc.Vector([0,1,0]))


### "earth retaining structure" spring.
elements.defaultMaterial= k.name
spring= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))
spring.setupVectors(xc.Vector([1,0,0]),xc.Vector([0,1,0]))

# Solve 
solProc= predefined_solutions.PenaltyKrylovNewton(prb= feProblem, maxNumIter= 10, numSteps= 10, convergenceTestTol= 1e-4, printFlag= 0)
ok= solProc.solve()
if(ok!=0):
    lmsg.error('Can\'t solve')
    exit(1)

dispXN2= n2.getDisp[0]
ratio0= dispXN2

rightMaterialDisp= soilMaterialRight.getStrain()
rightMaterialForce= soilMaterialRight.getStress()
leftMaterialDisp= soilMaterialLeft.getStrain()
leftMaterialForce= soilMaterialLeft.getStress()

ratio1= abs(rightMaterialForce+44129.924999999996)/44129.924999999996
ratio2= abs(leftMaterialForce+44129.924999999996)/44129.924999999996

# Update element stiffness.
newDepth= 0.1 #2.99
## Compute the new yield stresses.
new_sg_v= rankineSoil.getVerticalStressAtDepth(z= newDepth)
newEa, newE0, newEp= rankineSoil.getEarthThrusts(sg_v= new_sg_v, tributaryArea= tributaryArea)
eyBasicMaterialLeft= soilMaterialLeft.material
eyBasicMaterialLeft.setParameters(Kh, -newEp, -newEa)
soilMaterialLeft.setInitialStress(-newE0)
zlLeft.revertToStart()


# Solve again
ok= solProc.solve()
if(ok!=0):
    lmsg.error('Can\'t solve')
    exit(1)

dispXN2B= n2.getDisp[0]
refDispXN2B= 0.14180225716294145 # Displacement in node 2 obtained by deactivating
                                 # the left element. See (test_soil_response_07.py)
NRight= zlRight.getResistingForce()[0]
rightMaterialDispB= soilMaterialRight.getStrain()
rightMaterialForceB= soilMaterialRight.getStress()
NLeft= zlLeft.getResistingForce()[0]
leftMaterialDispB= soilMaterialLeft.getStrain()
leftMaterialForceB= soilMaterialLeft.getStress()
NSpring= spring.getResistingForce()[0]

leftForceDecrement= (leftMaterialForce-leftMaterialForceB)/leftMaterialForce
leftForceDecrementRef= 0.9393081761006289
ratio3= abs(leftForceDecrement-leftForceDecrementRef)/leftForceDecrementRef
ratio4= abs(NSpring+(NRight-NLeft))/abs(NSpring)

'''
print('Initial state:')
print('  right material displacement: ', rightMaterialDisp*1e3, ' mm')
print('  right material force: ', rightMaterialForce/1e3, 'kN')
print('  dispXN2= ', dispXN2*1e3, 'mm')
print('  left material displacement: ', leftMaterialDisp*1e3, ' mm')
print('  left material force: ', leftMaterialForce/1e3, 'kN')
print('  ratio1= ', ratio1)
print('  ratio2= ', ratio2)
print('Decompression:')
print('  dispXN2B= ', dispXN2B, 'mm')
print('  NRight= ', NRight/1e3, 'kN')
print('  right material displacement: ', rightMaterialDispB*1e3, ' mm')
print('  right material force: ', rightMaterialForceB/1e3, 'kN')
print('  NLeft= ', NLeft/1e3, 'kN')
print('  left material displacement: ', leftMaterialDispB*1e3, ' mm')
print('  left material force: ', leftMaterialForceB/1e3, 'kN')
print('  ratio3= ', ratio3)
print('  NSpring= ', NSpring/1e3, ' kN')
print('  ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio0)<1e-12) and (abs(ratio1)<1e-4) and (abs(ratio2)<1e-4) and (abs(ratio3)<1e-2) and (abs(ratio4)<1e-5)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

