# -*- coding: utf-8 -*-
''' Miscellaneous test of wind loads according to IAPF-10.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2023, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
from actions.wind import IAPF_wind

# Test getRectangularShapeDragCoefficient
data= [0.25, 0.33, 0.5, 0.67, 1.0, 1.5, 2.0,3,4,100.0]
expectedResults= [2.1, 2.2, 2.2, 2.2, 2.0, 1.7, 1.4, 1.2, 1.1, 1.1]

err= 0.0
for d, er in zip(data, expectedResults):
    r= IAPF_wind.getRectangularShapeDragCoefficient(B= d, h= 1.0)
    err+= (r-er)**2

err= math.sqrt(err)
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
