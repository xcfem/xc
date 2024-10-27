# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

pth= geom.Polyline3d([geom.Pos3d(0,0,0), geom.Pos3d(1,0,0), geom.Pos3d(1,1,0)])

ft= geom.FrenetTrihedron(pth)

t0= ft.getTangent(0.0)
t0Ref= geom.Vector3d(1,0,0)
t1= ft.getTangent(1.0)
halfSqrt2= math.sqrt(2)/2.0
t1Ref= geom.Vector3d(halfSqrt2, halfSqrt2, 0)
t2= ft.getTangent(2.0)
t2Ref= geom.Vector3d(0,1,0)

err= math.sqrt((t0-t0Ref).getModulus2()+(t1-t1Ref).getModulus2()+(t2-t2Ref).getModulus2())

'''
print('t0= ', t0)
print('t1= ', t1)
print('t2= ', t2)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')











