# -*- coding: utf-8 -*-
'''Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom

points= [geom.Pos3d(5.601,0.0,30.087), geom.Pos3d(5.525,0.0,30.087), geom.Pos3d(5.525, 0.0, 28.837), geom.Pos3d(5.601,0.0,28.837)]
    
plg= geom.Polygon3d(points)
sg= geom.Segment3d(geom.Pos3d(5.449,0.0,30.0424), geom.Pos3d(5.525,0.0,30.0424))
clippedSegment= plg.clip(sg)

'''
print('polygon: ', plg)
print('original segment: ', sg, 'length: ', sg.getLength())
print('clipped segment: ', clippedSegment, ' exists: ', clippedSegment.exists, 'length: ', clippedSegment.getLength())
'''

check= (clippedSegment.exists==False) # Intersection doesn't exists.

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (check):
    print("test "+fname+": ok.")
else:
    print("test "+fname+": ERROR.")
