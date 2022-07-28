# -*- coding: utf-8 -*-
''' Trivial test for MultiLinear material.'''
from __future__ import print_function
fy= 2600 # Yield stress of the material.
E= 2.1e6 # Young modulus of the material.
Nsteps= 10 # number of analysis steps.

import math
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor
# Define materials
strainStressPoints= [(0,0), (1,1), (2,2), (3,3)]
mlinear= typical_materials.defMultiLinearMaterial(preprocessor, "mlinear", strainStressPoints)

numIntervals= mlinear.numPoints
ratio= numIntervals-len(strainStressPoints)

#print(numPoints)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
