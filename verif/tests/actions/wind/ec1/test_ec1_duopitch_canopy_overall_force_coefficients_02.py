# -*- coding: utf-8 -*-
''' Compute the overall force coefficients of a duopitch canopy according to 
    clause 7.3 and table 7.7 of EN 1991-1-4:2005.
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

alpha1= math.atan2(0.3, 3) # short cantilever.
alpha2= math.atan2(0.5, 5) # long cantilever.
roofAngleRadians= -(alpha1+alpha2)/2.0
degreeOfBlockage= 1.0

maxCf, minCf= ec1_wind.get_duopitch_canopy_overall_force_coefficients(roofAngleRadians= roofAngleRadians, degreeOfBlockage= degreeOfBlockage)

maxCfRef= (0.4-0.3)/(-10+5)*(math.degrees(roofAngleRadians)+5)+0.3
ratio1= (maxCf-maxCfRef)/maxCfRef
minCfRef= (-1.4+1.3)/(-10+5)*(math.degrees(roofAngleRadians)+5)-1.3
ratio2= (minCf-minCfRef)/minCfRef # See first second and third rows in table 7.7

'''
print('  roof angle: '+ str(math.degrees(roofAngleRadians))+'º')
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

