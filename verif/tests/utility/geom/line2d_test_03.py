# -*- coding: utf-8 -*-
'''Test the sortPointsAlong method.
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
    if(i!=0):
        points.append(geom.Pos2d(-i,-i))

sqrt2= math.sqrt(2)

l2d= geom.Line2d(points)

sortedPoints= l2d.sortPointsAlong(points)

# Check results.
err= 0.0
ptLambda0= sortedPoints[0][1]
for (pt, ptLambda) in sortedPoints[1:]:
    deltaLambda= (ptLambda-ptLambda0)/sqrt2
    ptLambda0= ptLambda
    err+= (deltaLambda-1.0)**2

err= math.sqrt(err)

# print(err)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
