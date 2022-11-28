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

u= geom.Vector3d(0,1,1)
alpha= -math.pi/4.0
xAxis= geom.Line3d(geom.Pos3d(0,0,0), geom.Pos3d(100,0,0))
rot3dA= geom.Rotation3d(xAxis, alpha)
rot3dB= geom.Rotation3d(xAxis, -alpha)
rot3dC= rot3dA*rot3dB
v= rot3dC.getTrfVector3d(u)

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









