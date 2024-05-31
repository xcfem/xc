# -*- coding: utf-8 -*-
''' Use initial strain with elastic perfectly plastic material.'''

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

fy= 2600 # Yield stress of the material.
E= 2.1e6 # Young modulus of the material.
initStrain= -1e-4 # Initial strain.

# Definition of the FE model.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Define materials
epp= typical_materials.defElasticPPMaterial(preprocessor, name= "epp", E= E, fyp= 0.2*fy, fyn= -fy, initStrain= initStrain)

# Check the tension returned.
epp.setTrialStrain(0.0,0.0)
epp.commitState()
stress= epp.getStress()
refStress= -initStrain*E
err= abs(stress-refStress)/refStress

'''
print(stress)
print(refStress)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')    
