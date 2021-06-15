# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
axes= geom.PrincipalAxesOfInertia2D(geom.Pos2d(1,2),1.0,1.0,0.0)

i1= axes.I1
i2= axes.I2

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(i1==1. and i2==1.):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
