# -*- coding: utf-8 -*-
''' Compute the overall force coefficients of a monopitch canopy according to 
    clause 7.3 and table 7.6 of EN 1991-1-4:2005.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
from actions.wind import ec1_wind

roofAngleRadians= math.radians(20)
degreeOfBlockage= 0.5

maxCf, minCf= ec1_wind.get_monopitch_canopy_overall_force_coefficients(roofAngleRadians= roofAngleRadians, degreeOfBlockage= degreeOfBlockage)

ratio1= (maxCf-0.8)/0.8 # See 13th row in table 7.6
minCfRef= (-1.3-1.4)/2.0
ratio2= (minCf-minCfRef)/minCfRef # See first second and third rows in table 7.6

'''
print('maxCf= ', maxCf)
print("ratio1= ", ratio1)
print('minCf= ', minCf)
print("ratio2= ", ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-8) and (abs(ratio2)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

