#!/usr/bin/env python
''' Dumb test of the VelNormalFrcDep friction model.

see https://opensees.berkeley.edu/wiki/index.php/Velocity_and_Normal_Force_Dependent_Friction
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import friction_models as fm


# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define a normal force depenent  friction model
muSlow= 0.12 # coefficient of friction at low velocity. 
muFast= 0.18 # coefficient of friction at high velocity. 
nSlow= 0.8 # exponent for coefficient of friction at low velocity.
nFast= 0.7 # exponent for coefficient of friction at high velocity. 
alpha0= 25.0 # constant rate parameter coefficient.
alpha1= 0.0 # linear rate parameter coefficient.
alpha2= 0.0 # quadratic rate parameter coefficient. 
maxMuFact= 3 # factor for determining the maximum coefficient of friction.
             # This value prevents the friction coefficient from exceeding an
             # unrealistic maximum value when the normal force becomes very
             # small. The maximum friction coefficient is determined from
             # μFast, for example μ ≤ $maxMuFac*μFast. 
W= 1000.0

aSlow= muSlow/math.pow(W,nSlow-1) # constant for coefficient of friction at low velocity.
aFast= muFast/math.pow(W,nFast-1) # constant for coefficient of friction at high velocity.


frictionModel= fm.def_normal_force_dependent_friction_model(preprocessor, name= None, aSlow= aSlow, nSlow= nSlow, aFast= aFast, nFast= nFast, alpha0= alpha0, alpha1= alpha1, alpha2= alpha2, maxMuFact= maxMuFact)

# Check results.
ratio1= abs(frictionModel.aSlow-aSlow)/aSlow
ratio2= abs(frictionModel.aFast-aFast)/aFast
ratio3= abs(frictionModel.aSlow-aSlow)/aSlow
ratio4= abs(frictionModel.aFast-aFast)/aFast
ratio5= abs(frictionModel.alpha0-alpha0)/alpha0
ratio6= abs(frictionModel.alpha1-alpha1)
ratio7= abs(frictionModel.alpha2-alpha2)
ratio8= abs(frictionModel.maxMuFact-maxMuFact)/maxMuFact

'''
print(ratio1)
print(ratio2)
print(ratio3)
print(ratio4)
print(ratio5)
print(ratio6)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) and (abs(ratio1)<1e-15) and (abs(ratio3)<1e-15) and (abs(ratio4)<1e-15) and (abs(ratio5)<1e-15) and (abs(ratio6)<1e-15) and (abs(ratio7)<1e-15) and (abs(ratio8)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

