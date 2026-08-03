#!/usr/bin/env python
''' Dumb test of the VelDependent friction model.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import friction_models as fm

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define a velocity-pressure-dependent friction model
muSlow= 0.085 # Coefficient of friction at low sliding velocity.
muFast= 0.163 # Coefficient of friction at high sliding velocity for reference pressure.
A= 7.0686 # Constant parameter for variation of friction with velocity.
deltaMu= 0.05 # Difference between fast and slow friction parameters.
alpha= 0.08 # Pressure dependency parameter.
transRate= 0.77 # Transition rate parameter from low to high velocity.
frictionModel= fm.def_velocity_pressure_dependent_friction_model(preprocessor, name= None, muSlow = muSlow, muFast = muFast, transRate= transRate, nominalContactArea= A, deltaMu= deltaMu, alpha= alpha)

# Check results.
ratio1= abs(frictionModel.muSlow-muSlow)/muSlow
ratio2= abs(frictionModel.muFast-muFast)/muFast
ratio3= abs(frictionModel.transRate-transRate)/transRate
ratio4= abs(frictionModel.A-A)/A
ratio5= abs(frictionModel.deltaMu-deltaMu)/deltaMu
ratio6= abs(frictionModel.alpha-alpha)/alpha

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
if (abs(ratio1)<1e-15) and (abs(ratio1)<1e-15) and (abs(ratio3)<1e-15) and (abs(ratio4)<1e-15)and (abs(ratio5)<1e-15) and (abs(ratio6)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

