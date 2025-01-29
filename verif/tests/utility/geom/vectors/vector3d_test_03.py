# -*- coding: utf-8 -*-
'''Angle between two vectors. Example 1.6 of the thesis «La teoría de bloque aplicada a la dinámica de rocas» by Juan Carlos Ayes Zamudio.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

vec1= geom.Vector3d(1,0,0)
vec2= geom.Vector3d(-1,0,0)
delta12= vec1.getAngle(vec2)
vec3= geom.Vector3d(-1,-1,0)
delta13= vec1.getAngle(vec3)



delta12Teor= math.pi
delta13Teor= 3/4.0*math.pi
ratio1= math.fabs(delta12Teor-delta12)/delta12Teor
ratio2= math.fabs(delta13Teor-delta13)/delta13Teor

'''
print(math.degrees(delta12), math.degrees(delta12Teor), ratio1)
print(math.degrees(delta13), math.degrees(delta13Teor), ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-8 and math.fabs(ratio2)<1e-8:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')











