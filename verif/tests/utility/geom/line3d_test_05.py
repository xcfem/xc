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
points= [geom.Pos3d(4367.18, 5275.78, 10.6901), geom.Pos3d(4365.84, 5274.04, 10.6963), geom.Pos3d(4363.13, 5270.52, 10.7088), geom.Pos3d(4362.76, 5270.04, 10.7105), geom.Pos3d(4362.76, 5270.04, 10.7105), geom.Pos3d(4362.18, 5269.29, 10.7131), geom.Pos3d(4362.18, 5269.29, 10.7131), geom.Pos3d(4361.43, 5268.31, 10.7166), geom.Pos3d(4366.61, 5275.04, 10.6927), geom.Pos3d(4367.17, 5275.78, 10.6901), geom.Pos3d(4365.84, 5274.04, 10.6963), geom.Pos3d(4366.61, 5275.04, 10.6927), geom.Pos3d(4363.13, 5270.52, 10.7088), geom.Pos3d(4362.76, 5270.04, 10.7105)]

l3d= geom.Line3d(points)

sortedPoints= l3d.sortPointsAlong(points)


# Remove points too close.
filteredPoints= [(sortedPoints[0][0], sortedPoints[0][1])]
ptLambda0= sortedPoints[0][1]
for (pt, ptLambda) in sortedPoints[1:]:
    deltaLambda= (ptLambda-ptLambda0)
    ptLambda0= ptLambda
    if(deltaLambda>1e-2):
        filteredPoints.append((pt, ptLambda))

# Check results.
err= 0.0
refValues= [-4.6346473299365, -3.4005886974115893, -2.452484250544681, -1.8464292219490297, 2.595941307425775, 3.8580478375788276, 4.786029153999012]
for (pt, ptLambda), refLambda in zip(filteredPoints, refValues):
    err+= (ptLambda-refLambda)**2
err= math.sqrt(err)

# print(err)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
