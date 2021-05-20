# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
vec1=geom.Vector3d(1,2,3)
xV=vec1.x
yV=vec1.y
zV=vec1.z

tp=vec1.tipo()

import os
fname= os.path.basename(__file__)
if xV==1 and yV==2 and zV==3:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")












