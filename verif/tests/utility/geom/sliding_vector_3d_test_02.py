# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
import math

pos1=geom.Pos3d(0.5,0,0)
ptoAplica=geom.Pos3d(5,0,0)
vectorDir=geom.Vector3d(0,2,0)
vec1=geom.SlidingVector3d(ptoAplica,vectorDir)

brazo=vec1.getOrg().x-pos1.x
m=vec1.getMomentPos3d(pos1)

fuerza=vec1.y
mTeor=fuerza*brazo

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if m.x==0 and m.y==0and m.z==mTeor:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


