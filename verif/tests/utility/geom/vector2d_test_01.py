# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
import math

u=geom.Vector2d(1,0)
v=geom.Vector2d(1,1)
alpha=v.getAngle(u)

ratio1=math.degrees(alpha)-45

v.x=0
alpha=v.getAngle(u)
ratio2=math.degrees(alpha)-90
v.x=-1
alpha=v.getAngle(u)
ratio3=math.degrees(alpha)-135
v.y=0
alpha=v.getAngle(u)
ratio4=math.degrees(alpha)-180
v.y=-1
alpha=v.getAngle(u)
ratio5=math.degrees(alpha)-225
v.x=0 
alpha=v.getAngle(u)
ratio6=math.degrees(alpha)-270
v.x=1 
alpha=v.getAngle(u)
ratio7=math.degrees(alpha)-315
v.y=0 
alpha=v.getAngle(u)
ratio8=math.degrees(alpha)

import os
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-10 and math.fabs(ratio2)<1e-10 and math.fabs(ratio3)<1e-10 and math.fabs(ratio4)<1e-10 and math.fabs(ratio5)<1e-10 and math.fabs(ratio6)<1e-10 and math.fabs(ratio7)<1e-10 and math.fabs(ratio8)<1e-10:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")











