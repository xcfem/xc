# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import geom
import math

O=geom.Pos2d(0,0)
A=geom.Pos2d(1,0)
V=geom.Vector2d(0,1)

svs1= geom.SlidingVectorsSystem2d(A,V,0.0)
pto= svs1.getOrg()
momO= svs1.getMomentPos2d(O)

svs2= svs1.reduceTo(O)
momA= svs2.getMomentPos2d(A)
ratio1= momA

'''
print(pto)
print(momO)
print(momA.getModulus())
'''

import os
fname= os.path.basename(__file__)
if ratio1<1e-12:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")

