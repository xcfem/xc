# -*- coding: utf-8 -*-
''' Test section modulus computation for a trivial case.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

#    
#  p4  +--------------------+ p3
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      +--------------------+
#     p1                     p2
#

# Create the quadrilateral surface
sideLength= 1
p1= geom.Pos2d(5, 5)
p2= geom.Pos2d(5+sideLength, 5)
p3= geom.Pos2d(5+sideLength, 5+sideLength)
p4= geom.Pos2d(5, 5+sideLength)
quad= geom.Quadrilateral2d(p1, p2, p3, p4)

# Compute moduli.
Wx= quad.getWx()
I= 1/12.0*sideLength**4
WxRef= I/0.5
Wy= quad.getWy()
WyRef= WxRef

err= math.sqrt((Wx-WxRef)**2+(Wy-WyRef)**2)

# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
