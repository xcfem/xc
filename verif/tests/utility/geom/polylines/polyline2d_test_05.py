# -*- coding: utf-8 -*-
''' Offset method test on 2D polylines.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom

# Points.
#
#    +--------------+
#                   |
#        +------+   |
#               |   |
#               |   |
#        +------+   |
#                   |
#    +--------------+
#
vertices= [geom.Pos2d(0, 0), geom.Pos2d(1, 0), geom.Pos2d(1,1), geom.Pos2d(0, 1)]

contour2d= geom.Polyline2d(vertices)
l= contour2d.getLength()

# compute a polyline parallel to the previous one, at a fixed distance
offsetValue= 0.25
interior= contour2d.offset(-offsetValue)
lInt= interior.getLength()
lIntRef= l-4.0*offsetValue
ratio1= abs(lInt-lIntRef)/lInt

exterior= contour2d.offset(offsetValue)
lExt= exterior.getLength()
lExtRef= l+4.0*offsetValue
ratio2= abs(lExt-lExtRef)/lExt

'''
print(l)
print(lInt)
print(ratio1)
print(lExt)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


