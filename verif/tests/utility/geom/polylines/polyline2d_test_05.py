# -*- coding: utf-8 -*-
''' Test getPointAtLength.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

# Points.
#
#    +--------------+
#   x=1            x= 11 
#

# Vertex list.
vertices= [geom.Pos2d(1, 3.5), geom.Pos2d(11, 3.5)]

# Define polyline.
pline2d= geom.Polyline2d(vertices)

# Check results at different points.
lengths= [0.1,5.0,9.0]
## Reference values.
pointsRef= [geom.Pos2d(1.1, 3.5), geom.Pos2d(6.0, 3.5), geom.Pos2d(10.0, 3.5)]
err= 0.0

for l, pr in zip(lengths, pointsRef):
    p= pline2d.getPointAtLength(l) # point at s= l
    err+= (p-pr).getModulus()**2

err= math.sqrt(err) # average quadratic error

#print('err= ', err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
