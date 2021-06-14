# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
import math

def getAbs(vt1):
  return vt1.getModulus()

def getAngle(vt1,vt2):
  return vt1.getAngle(vt2)


u=geom.Vector2d(1,0)
v=geom.Vector2d(1,1)
modulus=getAbs(u)
alpha=getAngle(v,u)

ratio1= modulus-1
ratio2=math.degrees(alpha)-45



import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-10 and math.fabs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')











