# -*- coding: utf-8 -*-
'''Compute the intersection of two straight lines.'''

from __future__ import print_function
import xc_base
import geom
import math

r1=geom.Line2d(geom.Pos2d(1.,0.),geom.Pos2d(1.,1.))
r2=geom.Line2d(geom.Pos2d(0.,1.),geom.Pos2d(1.,1.))

#In lines doesn't intersect the list well be empty.
p= r1.getIntersectionWithLine(r2)[0] 

ratio1= math.fabs(p.x-1.0)
ratio2= math.fabs(p.y-1.0)

'''
print('p= ', p)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-20 and ratio2<1e-20 :
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')













