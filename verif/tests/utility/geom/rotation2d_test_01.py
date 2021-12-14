# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

u=geom.Vector2d(1,1)
alpha=math.radians(90)
rot2d= geom.Rotation2d(alpha)
v= rot2d.getTrfVector2d(u)

ratio1= abs(v.x+u.y)
ratio2= abs(v.y-u.x)

# print("u= ", u.x, u.y)
# print("v= ", v.x, v.y)
# print("ratio1= ",ratio1)
# print("ratio2= ",ratio2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')









