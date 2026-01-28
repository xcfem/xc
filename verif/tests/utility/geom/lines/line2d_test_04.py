# -*- coding: utf-8 -*-
'''Test the getProjection method.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

line= geom.Line2d(geom.Pos2d(0,0), geom.Pos2d(100,0))
pos= geom.Pos2d(1,1)

proj= line.getProjection(pos)
error= math.sqrt((proj.x-1)**2+(proj.y)**2)

# print(proj, error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
