# -*- coding: utf-8 -*-
'''Test the getParallel method.
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

segment= geom.Segment3d(geom.Pos3d(0,0,0), geom.Pos3d(10,0, 0))
pos= geom.Pos3d(1,1,1)

parallelLine= segment.getParallel(pos)

# Check results.
d1= parallelLine.dist(pos)
diffDir= (segment.getVDir().normalized()-parallelLine.getVDir().normalized()).getModulus()
error= math.sqrt(d1**2+diffDir**2)

'''
print(d1)
print(diffDir)
print(error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


