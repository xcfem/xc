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

# Define a velocity-dependent friction model
muSlow = 0.05
muFast = 0.163
transitionRate = 0.615
frictionModel= fm.def_velocity_dependent_friction_model(preprocessor, name= None, muSlow = muSlow, muFast = muFast, transRate= transitionRate)

ratio1= abs(frictionModel.muSlow-muSlow)/muSlow
ratio2= abs(frictionModel.muFast-muFast)/muFast
ratio3= abs(frictionModel.transRate-transitionRate)/transitionRate

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) and (abs(ratio1)<1e-15) and (abs(ratio3)<1e-15) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

