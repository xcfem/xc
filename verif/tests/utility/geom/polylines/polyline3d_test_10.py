# -*- coding: utf-8 -*-
'''Test the sortPointsAlong method on 3D polylines.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

points= [geom.Pos3d(0,0,0)]
for i in range(0,10):
    sign= (-1)**i
    pt= geom.Pos3d(1+i,0,sign)
    points.append(pt)
pline= geom.Polyline3d(points)

xi= [6.5, 5.5, 4.5, 3.5, 2.5, 1.5]
sample_points= list()
for x in xi:
    sample_points.append(geom.Pos3d(x,0.0,0.0))

sorted_points= pline.sortPointsAlong(sample_points)

err= 0.0
for x_ref, pt in zip(reversed(xi), sorted_points):
    err+= (x_ref-pt[0].x)**2

err= math.sqrt(err)

'''
print(sorted_points)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-8):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

    
