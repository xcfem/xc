# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom

pA= geom.Pos2d(0.0,0.0)
pB= geom.Pos2d(1.0,0.0); vB= geom.Vector2d(-1e3, 1e3)

vec=geom.SlidingVector2d(pB,vB)
Mvec= vec.getMoment(pA)
ratio1= abs(Mvec-1000)/1000

# print('Mvec= ', Mvec)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


