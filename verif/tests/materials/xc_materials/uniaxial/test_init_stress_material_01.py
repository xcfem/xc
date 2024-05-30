# -*- coding: utf-8 -*-
'''InitStressMaterial trivial test.''' 

from __future__ import print_function

import numpy as np
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
mat1= typical_materials.defElasticMaterial(preprocessor, "mat1", E= 1e6)
mat2= typical_materials.defInitStressMaterial(preprocessor, "mat2", mat1.name)

initStress= 1e3
mat2.initialStress= initStress
#mat2.setTrialStress(0.0,0.0)
#mat2.commitState()

stress= mat2.getStress()
refStress= initStress
ratio= abs(refStress-stress)/refStress

'''
print('E= ', mat1.E)
print('stress= ', stress)
print('ratio= ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

