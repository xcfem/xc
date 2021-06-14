# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
v1=geom.Vector3d(0,1,0)
v2=geom.Vector3d(-1,0,0)
v3=geom.Vector3d(0,0,1)

v= geom.Vector3d(1,1,1)
sc= geom.CooSysRect3d3d(v1,v2,v3)
vTrf= sc.getLocalCoordinates(v)
vTrfTeor= geom.Vector3d(1,-1,1)
ratio= (vTrf-vTrfTeor).getModulus()

#print("vTrf= ", vTrf)
#print("ratio= ", ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')










