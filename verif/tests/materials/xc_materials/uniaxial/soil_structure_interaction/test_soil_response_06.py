# -*- coding: utf-8 -*-
''' Pressure-displacement diagram verification test. Check equilibrium
with soil material at both sides of the node.'''

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
## Nonlinear spring material
nlSpringMaterial= typical_materials.defHorizontalSoilReactionMaterial(preprocessor, name= "soilResponse", samplePoints= samplePoints, initStrain= -initStrain, noTension= True)

## Problem type
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

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


# Solve 
solProc= predefined_solutions.PenaltyKrylovNewton(prb= feProblem, numSteps= 10)
solProc.solve()

dispXN2= n2.getDisp[0]
ratio0= dispXN2

rightMaterialStrain= soilMaterialRight.getStrain()
rightMaterialStress= soilMaterialRight.getStress()
leftMaterialStrain= soilMaterialLeft.getStrain()
leftMaterialStress= soilMaterialLeft.getStress()

ratio1= abs(rightMaterialStress+44129.924999999996)/44129.924999999996
ratio2= abs(leftMaterialStress+44129.924999999996)/44129.924999999996

'''
print('\nright material strain: ', rightMaterialStrain)
print('right material stress: ', rightMaterialStress)
print('dispXN2= ', dispXN2)
print('left material strain: ', leftMaterialStrain)
print('left material stress: ', leftMaterialStress)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio0)<1e-12 and abs(ratio1)<1e-4 and abs(ratio2)<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')




