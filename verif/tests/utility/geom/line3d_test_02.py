# -*- coding: utf-8 -*-
'''Test the regularize() method of Line3d. Home cooked test.

The regularize method checks if the line origin has very large 
coordinates (>1e6) and, in that case, replace it with the intersection 
of the line with one of the coordinate planes. Otherwise the behaviour of 
the line gets dominated by the large values of those coordinates which 
results in an erratic one.
'''

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

l3d= geom.Line3d(geom.Pos3d(-5.99162e+28,-7.98244e+10,-0), geom.Vector3d(-1, -1.33227e-18, -4.96467e-28))

lmbd= 100
pt1= l3d.getPoint(0)
pt2= l3d.getPoint(lmbd)

dist= pt1.dist(pt2)
ratio= abs(dist-lmbd)/lmbd

#print('line: ', line)

#print('dist: ', dist)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
