# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
import math

u=geom.Vector2d(1,0)
fromPoint=geom.Pos2d(0,0)
toPoint=geom.Pos2d(1,1)
s=geom.Segment2d(fromPoint,toPoint)
alpha=s.angleVector(u)
ratio1= math.degrees(alpha)-45

toPoint=geom.Pos2d(0,1)
s=geom.Segment2d(fromPoint,toPoint)
alpha=s.angleVector(u)
ratio2= math.degrees(alpha)-90

toPoint=geom.Pos2d(-1,1)
s=geom.Segment2d(fromPoint,toPoint)
alpha=s.angleVector(u)
ratio3= math.degrees(alpha)-135

toPoint=geom.Pos2d(-1,0)
s=geom.Segment2d(fromPoint,toPoint)
alpha=s.angleVector(u)
ratio4= math.degrees(alpha)-180

toPoint=geom.Pos2d(-1,-1)
s=geom.Segment2d(fromPoint,toPoint)
alpha=s.angleVector(u)
ratio5= math.degrees(alpha)-225

toPoint=geom.Pos2d(0,-1)
s=geom.Segment2d(fromPoint,toPoint)
alpha=s.angleVector(u)
ratio6= math.degrees(alpha)-270

toPoint=geom.Pos2d(1,-1)
s=geom.Segment2d(fromPoint,toPoint)
alpha=s.angleVector(u)
ratio7= math.degrees(alpha)-315

toPoint=geom.Pos2d(1,0)
s=geom.Segment2d(fromPoint,toPoint)
alpha=s.angleVector(u)
ratio8= math.degrees(alpha)

import os
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-10 and math.fabs(ratio2)<1e-10 and math.fabs(ratio3)<1e-10 and math.fabs(ratio4)<1e-10 and math.fabs(ratio5)<1e-10 and math.fabs(ratio6)<1e-10 and math.fabs(ratio7)<1e-10 and math.fabs(ratio8)<1e-10:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
