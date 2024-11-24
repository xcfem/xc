# -*- coding: utf-8 -*-
''' Use elastic perfectly plastic material to model a soil. 
    Check at-rest pressure.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from geotechnics import earth_pressure
from model import predefined_spaces
from materials import typical_materials

# Define soil model.
phi= math.pi/6.0
rankineSoil= earth_pressure.RankineSoil(phi, rho= 2000, Kh= 15e5)

# Define depth and tributary area corresponding to the spring simulating
# the soil reaction.
depth= 3.0 # 3 m
tributaryArea= 1.5 # 1.5 m2

# FE problem definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
## Problem type
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodes)

## Nonlinear spring material
sg_v= rankineSoil.getVerticalStressAtDepth(z= depth)
nlSpringMaterial= rankineSoil.defHorizontalSubgradeReactionNlMaterial(preprocessor, name= 'nlSpringMaterial', sg_v= sg_v, tributaryArea= tributaryArea)

nlSpringMaterial.setTrialStrain(0.0, 0.0)
nlSpringMaterial.commitState()
E0= rankineSoil.K0Jaky()*depth*rankineSoil.gamma()*tributaryArea
err= (nlSpringMaterial.getStress()+E0)/E0

# print(nlSpringMaterial.getStress()/1e3, -E0/1e3, err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')    
