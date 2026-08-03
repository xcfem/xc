#!/usr/bin/env python
''' Dumb test of the Coulomb friction model.'''

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

# Define coulomb friction model.
mu= 0.163
frictionModel= fm.def_coulomb_friction_model(preprocessor, name= None, mu= mu)

ratio= abs(frictionModel.mu-mu)/mu

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

