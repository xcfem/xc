# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
import math

u=geom.Vector2d(1,1)
alpha=math.radians(90)
rot2d= geom.Rotation2d(alpha)
v= rot2d.getTrfVector2d(u)

ratio1= abs(v.x+u.y)
ratio2= abs(v.y-u.x)

# print("u= ", u.x, u.y)
# print("v= ", v.x, v.y)
# print("ratio1= ",ratio1)
# print("ratio2= ",ratio2)

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")









