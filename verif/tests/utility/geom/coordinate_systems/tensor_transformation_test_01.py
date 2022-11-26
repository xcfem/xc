# -*- coding: utf-8 -*-
''' Trivial test of tensor transformation code.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import numpy as np
from geom_utils import tensor_transformation as tt
from numpy import linalg

tensorCylindrical= np.array([[1, 0, 0],
                             [0, 1, 0],
                             [0, 0, 1]])

# Transformed tensor.
tensorCartesian= tt.cylindrical_to_cartesian(tensorCylindrical= tensorCylindrical, theta= math.pi/4.0)

# Check result.
err= linalg.norm(tensorCartesian-tensorCylindrical)

'''
print(tensorCartesian)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-10):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


