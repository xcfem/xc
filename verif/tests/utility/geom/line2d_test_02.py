# -*- coding: utf-8 -*-
'''Test the constructor from a point list

This constructor is based on least squares method.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

points= list()
for i in range(0,50):
    points.append(geom.Pos2d(i,i))

# Test constructor from point list (least squares method).
l2d= geom.Line2d(points)

# Check results.
iVector= l2d.getIVector
err= abs(iVector.x-iVector.y)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
