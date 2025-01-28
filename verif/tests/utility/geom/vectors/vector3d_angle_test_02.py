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

vec1= geom.Vector3d(1,0,0)

angles= [0.0, 30.0, 45.0, 60.0, 90.0, 120.0, 135.0, 150.0, 180.0, 210.0, 225.0, 240.0, 270.0, 300.0, 315.0, 330.0, 360.0]

computedAngles= list()
error= 0.0
twoPi= 2*math.pi
for a in angles:
    angRad= math.radians(a)
    vec2= geom.Vector3d(math.cos(angRad), math.sin(angRad),0.0)
    computedAngle= vec1.getAngle(vec2)
    err= computedAngle-angRad
    if(angRad>math.pi):
        err= twoPi-computedAngle-angRad
    error+= err**2
    computedAngles.append(computedAngle)

error= math.sqrt(error)

# for a1, a2 in zip(angles, computedAngles):
#     print(a1, math.degrees(a2))
# print("error= ", error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-6:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')











