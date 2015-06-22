# -*- coding: utf-8 -*-

import xc_base
import geom
from geom_utils import parabola as pb
import math

e1= 0.5
e2= 0.0
f=0.25
L= 10.0
P= 10

uTeor= 8*P*f/L**2

p0= geom.Pos2d(0,e1)
p1= geom.Pos2d(L/2.0,(e1+e2)/2.0-f)
p2= geom.Pos2d(L,e2)

traceCable= pb.Parabola(p0,p1,p2)
 
x= 0.0
err= 0
nPts= 5
for i in range(1,nPts+2):
  x= float(i-1)*L/float(nPts)
  alpha= traceCable.alpha(x)
  r= traceCable.curvature(x)
  Px= P/math.cos(alpha)*geom.Vector2d(math.cos(alpha),math.sin(alpha))
  u= (P/math.cos(alpha))*r
  err+= (u-uTeor)**2

err= math.sqrt(err)

#print "err= ", err
  
import os
fname= os.path.basename(__file__)
if (err<0.01):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
