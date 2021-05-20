# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
import math

r1=geom.Line2d(geom.Pos2d(0.,0.),geom.Pos2d(10.,0.))
r2=geom.Line2d(geom.Pos2d(0.,-2.),geom.Pos2d(10.,-2.))
r3=geom.Line2d(geom.Pos2d(0.,2.),geom.Pos2d(10.,2.))

a=r2.getParamA()
b=r2.getParamB()

ratio1=math.fabs(a)
ratio2=math.fabs(b+2)

a=r3.getParamA()
b=r3.getParamB()

ratio3=math.fabs(a)
ratio4=math.fabs(b-2)

import os
fname= os.path.basename(__file__)
if ratio1<1e-15 and ratio2<1e-15 and ratio3<1e-15 and ratio4<1e-15 :
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")













