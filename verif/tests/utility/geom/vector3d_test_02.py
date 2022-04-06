# -*- coding: utf-8 -*-
'''Angle between two vectors. Example 1.6 of the thesis «La teoría de bloque aplicada a la dinámica de rocas» by Juan Carlos Ayes Zamudio.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

vec1= geom.Vector3d(9,8,7)
vec2= geom.Vector3d(1,2,1)
delta= vec1.getAngle(vec2)
deltaTeor= math.acos(32/2.4495/13.9284)
ratio1= math.fabs(deltaTeor-delta)/deltaTeor

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-4:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')











