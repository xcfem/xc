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
#
#  4 +--------------------+ 3
#    |                    |
#    |         +O         |
#    |                    |
#  1 +--------------------+ 2
#
vPoint= geom.Pos3d(0,0,100)
L= 10
skew= 0.999
p1= geom.Pos3d(0,0,0)
p2= geom.Pos3d(L,0,skew*L)
p3= geom.Pos3d(L,L,skew*L)
p4= geom.Pos3d(0,L,0)
skew= 1.001
p11= geom.Pos3d(0,0,0)
p12= geom.Pos3d(L,0,skew*L)
p13= geom.Pos3d(L,L,skew*L)
p14= geom.Pos3d(0,L,0)
skew= 1.5
p21= geom.Pos3d(0,0,0)
p22= geom.Pos3d(L,0,skew*L)
p23= geom.Pos3d(L,L,skew*L)
p24= geom.Pos3d(0,L,0)

pointsA= geom.ListPos3d([p1,p2,p3,p4])
pointsB= geom.ListPos3d([p11,p12,p13,p14])
pointsC= geom.ListPos3d([p21,p22,p23,p24])
pointsD= geom.ListPos3d([p4,p3,p2,p1])


pointsAOrientation= pointsA.orientation(vPoint)
pointsBOrientation= pointsB.orientation(vPoint)
pointsCOrientation= pointsB.orientation(vPoint)
pointsDOrientation= pointsD.orientation(vPoint)
ok= (pointsAOrientation=="counterclockwise") and (pointsBOrientation=="counterclockwise") and (pointsCOrientation=="counterclockwise") and (pointsDOrientation=="clockwise")

'''
print('points A orientation: ', pointsAOrientation, pointsA.getAvgKVector())
print('points B orientation: ', pointsBOrientation)
print('points C orientation: ', pointsCOrientation)
print('points D orientation: ', pointsDOrientation, pointsD.getAvgKVector())
'''

import os
fname= os.path.basename(__file__)
if ok:
  print("test "+fname+": ok.")
else:
  print("test "+fname+": ERROR.")
