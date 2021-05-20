# -*- coding: utf-8 -*-
from __future__ import print_function
import xc_base
import geom
svd1=geom.SlidingVectorsSystem2d()
ptoAplic=geom.Pos2d(1,1)
vectorDir=geom.Vector2d(1,0)
vec=geom.SlidingVector2d(ptoAplic,vectorDir)
svd1+=vec
ptoAplic=geom.Pos2d(1,-1)
vec=geom.SlidingVector2d(ptoAplic,vectorDir)
svd1+=vec

zml1=svd1.zeroMomentLine()
vDir1= zml1.getVDir()

svd2=geom.SlidingVectorsSystem2d()
ptoAplic=geom.Pos2d(0,0)
vectorDir=geom.Vector2d(0,1)
vec=geom.SlidingVector2d(ptoAplic,vectorDir)
svd2+=vec
ptoAplic=geom.Pos2d(-1,0)
vec=geom.SlidingVector2d(ptoAplic,vectorDir)
svd2+=vec

zml2=svd2.zeroMomentLine()
vDir2= zml2.getVDir()
ratio= (vDir1.x-1.0)**2+vDir1.y**2+vDir2.x**2+(vDir2.y-1.0)**2

# print('zml1= ', zml1)
# print('vDir1= ', vDir1)
# print('zml2= ', zml2)
# print('vDir2= ', vDir2)
# print('ratio= ', ratio)

import os
fname= os.path.basename(__file__)
if(ratio<1e-13):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")

