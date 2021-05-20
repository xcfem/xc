# -*- coding: utf-8 -*-
''' Test of cover calculations of a point inside a polygon.'''
from __future__ import print_function
from __future__ import division

import math
import xc_base
import geom

plg=geom.Polygon2d()

#        +--------+
#        |        |
#        |        |
#        |        |
#        |        |
#        +--------+

plg.appendVertex(geom.Pos2d(0,0))
plg.appendVertex(geom.Pos2d(1,0))
plg.appendVertex(geom.Pos2d(1,1))
plg.appendVertex(geom.Pos2d(0,1))
area= plg.getArea()

p= geom.Pos2d(0.5,0.5)
cover= plg.getCover(p)
cover2= plg.getCover(p,geom.Vector2d(0.0,-1.0))
cover3= plg.getCover(p,geom.Vector2d(1.0,-1.0))
cover3ref= math.sqrt(2)/2.0

ratio1= abs(area-1)
ratio2= abs(cover-0.5)/0.5
ratio3= abs(cover2-0.5)/0.5
ratio4= abs(cover3-cover3ref)/cover3ref

'''
print('area= ', area)
print('ratio1= ', ratio1)
print('cover= ', cover)
print('ratio2= ', ratio2)
print('cover2= ', cover2)
print('ratio3= ', ratio3)
print('cover3= ', cover3)
print('ratio4= ', ratio4)
'''

import os
fname= os.path.basename(__file__)
if(ratio1<1e-15 and ratio2<1e-15 and ratio3<1e-15 and ratio4<1e-15):
    print("test ",fname,": ok.")
else:
    print("test ",fname,": ERROR.")

