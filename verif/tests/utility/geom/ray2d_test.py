# -*- coding: utf-8 -*-
'''Compute the intersection of a line with a segment.'''

from __future__ import print_function
import xc_base
import geom
import math

sg= geom.Segment2d(geom.Pos2d(-0.443,0.443),geom.Pos2d(-0.443,-0.443))
sr= geom.Ray2d(geom.Pos2d(0.19855,-0.082042),geom.Pos2d(-39.7765,16.6642))
p= sg.getIntersectionWithRay(sr)[0] 

d= sr.dist(p)

#print('d2= ', d)

import os
fname= os.path.basename(__file__)
if(d<1e-15):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
