# -*- coding: utf-8 -*-
''' Closest pair of points algorithm verification test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

from geom_utils import closest_pair_of_points as cpp


points = [geom.Pos2d(2.0, 3.0), geom.Pos2d(12, 30), geom.Pos2d(40, 50), geom.Pos2d(5, 1), geom.Pos2d(12, 10), geom.Pos2d(3, 4)]

dist= cpp.closest_pair_of_points(points, len(points))
ratio1= abs(dist-1.4142135623730951)/1.4142135623730951

'''
print("Distance:", dist)
print("ratio1= ", ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
