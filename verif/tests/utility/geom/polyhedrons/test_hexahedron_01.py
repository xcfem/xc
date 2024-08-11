# -*- coding: utf-8 -*-
''' Trivial check of quadrilateral surfaces.'''

from __future__ import division
from __future__ import print_function

import geom

#
#     4 +---------+ 7     
#      /|        /|       
#     / |       / |       
#  5 +---------+6 |       
#    |  |      |  |       
#    |3 +------|--+ 2     
#    | /       | /
#    |/        |/
#  0 +---------+ 1
#

# Define k-points.
p0= geom.Pos3d(0,0,0)
p1= geom.Pos3d(1,0,0)
p2= geom.Pos3d(1,1,0)
p3= geom.Pos3d(0,1,0)

p4= p3+geom.Vector3d(0,0,1)
p5= p0+geom.Vector3d(0,0,1)
p6= p1+geom.Vector3d(0,0,1)
p7= p2+geom.Vector3d(0,0,1)

# Define hexahedron.
h= geom.Hexahedron3d(p0, p1, p2, p3, p4, p5, p6, p7)

v= h.volume
ratio1= abs(v-1)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-8):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

