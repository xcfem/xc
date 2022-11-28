# -*- coding: utf-8 -*-
''' Trivial test for 3D rotation transformation.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

u= geom.Vector2d(1,1)
alpha= -math.pi/4.0
rot2dA= geom.Rotation2d(alpha)
rot2dB= geom.Rotation2d(-alpha)
rot2dC= rot2dA*rot2dB
v= rot2dC.getTrfVector2d(u)

vRef= u

err= (v-vRef).getModulus()

'''
print("u= ", u)
print("v= ", v)
print("vRef= ", vRef)
print("err= ",err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-8:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')









