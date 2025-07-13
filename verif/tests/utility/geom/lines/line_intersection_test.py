# -*- coding: utf-8 -*-
'''Compute the intersection of a line with a segment.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

r= geom.Line2d(geom.Pos2d(0,0),geom.Pos2d(10,10))

#In lines doesn't intersect the list well be empty.
pA= r.getIntersection(1, 3)[0] 
pB= r.getIntersection(2, 3)[0]

ratio1= pA.dist(geom.Pos2d(3,3))
ratio2= pB.dist(pA)

'''
print('pA= ', pA)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-10 and ratio2<1e-10 :
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
