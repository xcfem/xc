# -*- coding: utf-8 -*-
'''Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom

vPoint= geom.Pos3d(0,0,100)
L= 10
skew= 0.999
p1= geom.Pos3d(L,0,0)
p2= geom.Pos3d(2*L,0,skew*L)
p3= geom.Pos3d(2*L,L,skew*L)
p4= geom.Pos3d(L,L,0)
skew= 1.001
p11= geom.Pos3d(L,0,0)
p12= geom.Pos3d(2*L,0,skew*L)
p13= geom.Pos3d(2*L,L,skew*L)
p14= geom.Pos3d(L,L,0)


plgA= geom.Polygon3d([p1,p2,p3,p4])
plgAK= plgA.getKVector()
plgAOrientation= plgA.orientation(vPoint)

plgB= geom.Polygon3d([p11,p12,p13,p14])
plgBK= plgB.getKVector()
plgBOrientation= plgB.orientation(vPoint)

plgC= geom.Polygon3d([p4,p3,p2,p1])
plgCK= plgC.getKVector()
plgCOrientation= plgC.orientation(vPoint)

ok= (plgAOrientation=="counterclockwise") and (plgBOrientation=="counterclockwise") and (plgCOrientation=="clockwise")

'''
print('polygon A k vector: ',plgAK, ' orientation: ', plgAOrientation)
print('polygon B k vector: ',plgBK, ' orientation: ', plgBOrientation)
print('polygon C k vector: ',plgCK, ' orientation: ', plgCOrientation)
'''

import os
fname= os.path.basename(__file__)
if ok:
  print("test "+fname+": ok.")
else:
  print("test "+fname+": ERROR.")
