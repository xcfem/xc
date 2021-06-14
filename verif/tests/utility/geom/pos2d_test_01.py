# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
pos1= geom.Pos2d(1,2)

xPt= pos1.x
yPt= pos1.y

#print("pos1.x= ",pos1.x," pos1.y= ",pos1.y)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(xPt==1. and yPt==2.):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
 
