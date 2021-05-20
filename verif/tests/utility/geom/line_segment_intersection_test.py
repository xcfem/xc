# -*- coding: utf-8 -*-
'''Compute the intersection of a line with a segment.'''

from __future__ import print_function
import xc_base
import geom
import math

s= geom.Segment2d(geom.Pos2d(0.443,0.),geom.Pos2d(0.443,0.443))
r= geom.Line2d(geom.Pos2d(1.0,0.0116016),geom.Pos2d(-4735.73,0.0116019))

#In lines doesn't intersect the list well be empty.
p= s.getIntersectionWithLine(r)[0] 

ratio1= math.fabs(p.x-0.443)
ratio2= math.fabs(p.y-0.0116016)

'''
print('p= ', p)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
'''

import os
fname= os.path.basename(__file__)
if ratio1<1e-20 and ratio2<1e-10 :
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
