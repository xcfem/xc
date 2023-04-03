# -*- coding: utf-8 -*-
''' Trivial test of the behaviour of PressureIndependentMultiYieldMaterial material class. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from materials import typical_materials
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

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
ratio1= abs(soil.rho-satDensity)/satDensity
ratio2= abs(soil.refShearModulus-shear)/shear
ratio3= abs(soil.refBulkModulus-bulk)/bulk
ratio4= abs(soil.frictionAngle-phi)
ratio5= abs(soil.refPressure-refPress)/refPress
ratio6= abs(soil.peakShearStrain-gammaPeak)/gammaPeak
refTangent= xc.Matrix([[653333.333333, 603333.333333, 0], [603333.333333, 653333.333333, 0], [0,0,25000]])
diffTangent= soil.getTangent()-refTangent
ratio7= diffTangent.Norm()/refTangent.Norm()
diffInitialTangent= soil.getInitialTangent()-refTangent
ratio8= diffInitialTangent.Norm()/refTangent.Norm()

'''
print('soil.rho= ', soil.rho)
print('ratio1= ', ratio1)
print('soil.refShearModulus= ', soil.refShearModulus)
print('ratio2= ', ratio2)
print('soil.refBulkModulus= ', soil.refBulkModulus)
print('ratio3= ', ratio3)
print('soil.frictionAngle= ', soil.frictionAngle)
print('ratio4= ', ratio4)
print('soil.refPressure= ', soil.refPressure)
print('ratio5= ', ratio5)
print('soil.peakShearStrain= ', soil.peakShearStrain)
print('ratio6= ', ratio6)
print('soil.tangent= ', soil.getTangent())
print('soil.tangent-refTangent= ', diffTangent)
print('ratio7= ', ratio7)
print('soil.inititalTangent= ', soil.getInitialTangent())
print('ratio8= ', ratio8)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) and (abs(ratio2)<1e-10) and (abs(ratio3)<1e-10) and (abs(ratio4)<1e-10) and (abs(ratio5)<1e-10) and (abs(ratio6)<1e-10) and (abs(ratio7)<1e-10) and (abs(ratio8)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
