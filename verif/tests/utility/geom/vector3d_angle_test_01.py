# -*- coding: utf-8 -*-
'''Angle between two vectors.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

vec1= geom.Vector3d(1,7.19412e-10,-2.08017e-05)
vec2= geom.Vector3d(1,0,-2.08017e-05)
angle= vec1.getAngle(vec2)
angleTeor= 0.0
ratio1= math.fabs(angleTeor-angle)#/angleTeor

#print("angle= ", math.degrees(angle))

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-4:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')











