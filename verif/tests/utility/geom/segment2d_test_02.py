# -*- coding: utf-8 -*-
''' Segment2d trivial test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

fromPoint=geom.Pos2d(0,0)
toPoint=geom.Pos2d(10,0)
s1=geom.Segment2d(fromPoint,toPoint)
s2=s1.offsetDouble(2)
s3=s1.offsetDouble(-2)

p1=s2.getCenterOfMass()
p2=s3.getCenterOfMass()

ratio1=math.fabs(p1.dist(p2)-4)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-15:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


