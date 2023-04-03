# -*- coding: utf-8 -*-
''' Trivial test of the behaviour of PressureIndependentMultiYieldMaterial material class. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials import typical_materials

# Define finite element problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define material.
satDensity= 2.3 # saturated mass density (Mg/m3)
H2ODensity= 1.0 # fluid mass density (Mg/m3)

shear= 2.5e4 # shear modulus (kPa)
bulk= 6.2e5 # bulk modulus (kPa)
uBulk= 2.2e5 # undrained bulk modulus (kPa)


cohesion = 45.0 # cohesion (kPa)
phi= 0.0 # friction angle (degrees)

gammaPeak= 0.1 # peak shear strain
refPress= -80.0 # reference pressure (compression, kPa)

pressCoeff = 0.0 # pressure dependency coefficient

numSurf= 22 # number of yield surfaces
soil= typical_materials.defPressureIndependentMultiYieldMaterial(preprocessor, name= '1', nd= 2, rho= satDensity, refShearModul= shear, refBulkModul= bulk, cohesi= cohesion, peakShearStra= gammaPeak, frictionAng= phi, refPress= refPress, pressDependCoe= pressCoeff, numberOfYieldSurf= numSurf, gredu= [])

# Define mesh
## Define nodes.

#         8
# 7 +-----+-----+ 9
#   |           |
#   |     5     |
# 4 +     +     +6
#   |           |
#   |           |
#   +-----+-----+
#   1     2     3
nodeHandler= preprocessor.getNodeHandler
nodeHandler.dimEspace= 2 #two coord. for each node (x,y).

### Define corner nodes.
nodeHandler.numDOFs= 3 #three DOF for each node (Ux,Uy, pore pressure).

n1= nodeHandler.newNodeXY(0.0,0.0)
n3= nodeHandler.newNodeXY(1.0,0.0)
n7= nodeHandler.newNodeXY(0.0,1.0)
n9= nodeHandler.newNodeXY(1.0,1.0)

### Define interior nodes
nodeHandler.numDOFs= 2 #two DOF for each node (Ux,Uy).
n2= nodeHandler.newNodeXY(0.5,0.0)
n4= nodeHandler.newNodeXY(0.0,0.5)
n5= nodeHandler.newNodeXY(0.5,0.5)
n6= nodeHandler.newNodeXY(1.0,0.5)
n8= nodeHandler.newNodeXY(0.5,1.0)

## Define element.
# 
#         p
# l +-----+-----+ k
#   |           |
#   |     r     |
# q +     +     + n   Order of the nodes in the element.
#   |           |
#   |           |
#   +-----+-----+
#   i     m     j
#
elementHandler= preprocessor.getElementHandler
elementHandler.defaultMaterial= soil.name
newElem= elementHandler.newElement("NineFourNodeQuadUP",xc.ID([n1.tag, n3.tag, n9.tag, n7.tag, n2.tag, n6.tag, n8.tag, n4.tag, n5.tag]))

err= 0.0
elemMaterials= newElem.physicalProperties.getVectorMaterials
for soil in elemMaterials:
    err+= (soil.rho-satDensity)**2
    err+= (soil.refShearModulus-shear)**2
    err+= (soil.refBulkModulus-bulk)**2
    err+= (soil.frictionAngle-phi)**2
    err+= (soil.refPressure-refPress)**2
    err+= (soil.peakShearStrain-gammaPeak)**2
    refTangent= xc.Matrix([[653333.333333, 603333.333333, 0], [603333.333333, 653333.333333, 0], [0,0,25000]])
    diffTangent= soil.getTangent()-refTangent
    err+= diffTangent.Norm2()
    diffInitialTangent= soil.getInitialTangent()-refTangent
    err+= diffInitialTangent.Norm2()

err= math.sqrt(err)
# print('err= ', err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(err)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
