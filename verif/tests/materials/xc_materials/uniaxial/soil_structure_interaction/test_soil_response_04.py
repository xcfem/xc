# -*- coding: utf-8 -*-
''' Pressure-displacement diagram verification test.'''

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

phi= math.pi/6.0
rankineSoil= earth_pressure.RankineSoil(phi, rho= 2000, Kh= 30e6)

depth= 3.0 # 3 m
tributaryArea= 1.5 # 1.5 m2

sg_v= rankineSoil.getVerticalStressAtDepth(z= depth)
samplePoints, initStrain= earth_pressure.get_horizontal_soil_reaction_diagram(sg_v= sg_v, tributaryArea= tributaryArea, Ka= rankineSoil.Ka(), K0= rankineSoil.K0Jaky(), Kp= rankineSoil.Kp(), Kh= rankineSoil.Kh)

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
## Material definition
soilResponse= typical_materials.defHorizontalSoilReactionMaterial(preprocessor, name= "soilResponse", samplePoints= samplePoints, initStrain= -initStrain)
# Spring opposed to soil movement.
k= typical_materials.defElasticMaterial(preprocessor, "k",E= 1e6)

## Problem type
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Define mesh.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0,0)

n3= nodes.newNodeXY(0,0)
n4= nodes.newNodeXY(0,0)


## Constraints
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF00(n2.tag)
modelSpace.fixNode000(n3.tag)
modelSpace.fixNodeF00(n4.tag)

## Define element.
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Dimension of element space

### Left element.
elements.defaultMaterial= soilResponse.name
zlLeft= elements.newElement("ZeroLength",xc.ID([n4.tag,n3.tag]))
soilMaterialLeft= zlLeft.getMaterials()[0]
zlLeft.setupVectors(xc.Vector([-1,0,0]),xc.Vector([0,-1,0]))
elements.defaultMaterial= k.name
leftSpring= elements.newElement("ZeroLength",xc.ID([n3.tag,n4.tag]))

### Right element.
elements.defaultMaterial= soilResponse.name
zlRight= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))
soilMaterialRight= zlRight.getMaterials()[0]
zlRight.setupVectors(xc.Vector([1,0,0]),xc.Vector([0,1,0]))
elements.defaultMaterial= k.name
rightSpring= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))

# Solve 
solProc= predefined_solutions.PenaltyKrylovNewton(prb= feProblem, numSteps= 10)
solProc.solve()
dispXN2= n2.getDisp[0]
dispXN4= n4.getDisp[0]
ratio0= abs(dispXN2+dispXN4)

rightMaterialStrain= soilMaterialRight.getStrain()
rightMaterialStress= soilMaterialRight.getStress()
leftMaterialStrain= soilMaterialLeft.getStrain()
leftMaterialStress= soilMaterialLeft.getStress()

ratio1= abs(rightMaterialStress+29303.922122326207)/29303.922122326207
ratio2= abs(leftMaterialStress+29303.922122326207)/29303.922122326207

'''
print('\nright material strain: ', rightMaterialStrain)
print('right material stress: ', rightMaterialStress)
print('dispXN2= ', dispXN2)
print('left material strain: ', leftMaterialStrain)
print('left material stress: ', leftMaterialStress)
print('dispXN4= ', dispXN4)
print('ratio0= ', ratio0)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio0)<1e-12 and abs(ratio1)<1e-4 and abs(ratio2)<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')




