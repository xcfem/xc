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
toPoint=geom.Pos2d(10,0)
s1=geom.Segment2d(fromPoint,toPoint)

# get buffer polygon (d= 1.0).
r= 1.0
numVertices= 8
pg= s1.getBufferPolygon(r, numVertices)
# Compute area.
area= pg.getArea()
# Compute theoretical area.
refArea= s1.getLength()*2.0*r+0.5*numVertices*r*r*math.sin(2*math.pi/numVertices)
# Compute relative error.
ratio= abs(area-refArea)/refArea

#print(area, refArea,ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-5):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


