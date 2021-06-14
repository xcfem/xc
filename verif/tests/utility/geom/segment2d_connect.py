# -*- coding: utf-8 -*-
''' Home made test: segment concatenation.'''

import xc_base
import geom

s1= geom.Segment2d(geom.Pos2d(0,0),geom.Pos2d(1,0))
s2= geom.Segment2d(geom.Pos2d(0,0),geom.Pos2d(0,1))
s3= geom.Segment2d(geom.Pos2d(1,0),geom.Pos2d(1,1))

s4= geom.Segment2d(geom.Pos2d(2,2),geom.Pos2d(-1,2))
s5= geom.Segment2d(geom.Pos2d(2,2),geom.Pos2d(2,-1))
s6= geom.Segment2d(geom.Pos2d(-1,2),geom.Pos2d(-1,-1))

polylines= geom.get_2d_polylines([s1,s2,s3,s4,s5,s6],.01)
ratio1= abs(polylines[0].getLength()-3)/3.0
ratio2= abs(polylines[1].getLength()-9)/9.0

'''
for pl in polylines:
  print('pline: ', pl)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print("test ",fname,": ok.")
else:
    print("test ",fname,": ERROR.")
