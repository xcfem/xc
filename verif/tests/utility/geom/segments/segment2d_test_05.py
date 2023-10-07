# -*- coding: utf-8 -*-
''' Segment2d trivial test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

# Define a line.
fromPoint=geom.Pos2d(0,0)
toPoint=geom.Pos2d(10,10)
s1 =geom.Segment2d(fromPoint,toPoint)

# get segment i vector.
iVector= s1.getIVector
jVector= s1.getJVector
sqrt2= math.sqrt(2)
halfSqrt2= sqrt2/2
err= (iVector.x-halfSqrt2)**2+(iVector.y-halfSqrt2)**2+(jVector.x-halfSqrt2)**2+(jVector.y+halfSqrt2)**2

# get a line parallel to the previous one, at a fixed distance
s2= s1.offset(1)
# the same, at the opposite side.
s3= s1.offset(-1)

vA= s3.getToPoint()-s2.getToPoint()
vB= s3.getFromPoint()-s2.getFromPoint()

err+= (vA.x+sqrt2)**2+(vA.y-sqrt2)**2+(vB.x+sqrt2)**2+(vB.y-sqrt2)**2
err= math.sqrt(err)

'''
print(iVector)
print(jVector)
print(err)
print(vA)
print(vB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-13):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


