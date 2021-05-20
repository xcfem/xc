# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
import math

r1=geom.Line3d(geom.Pos3d(0.,0.,0.),geom.Pos3d(10.,10.,10.))

vdir= r1.getVDir()
tp=r1.tipo()

import os
fname= os.path.basename(__file__)
if vdir.x==10. and vdir.y==10. and vdir.z==10.:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")















