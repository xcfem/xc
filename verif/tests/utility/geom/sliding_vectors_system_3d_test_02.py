# -*- coding: utf-8 -*-
from __future__ import print_function
import xc_base
import geom
pto=geom.Pos3d(-1.,1.,0.)

fromPoint=geom.Pos3d(-1.,1.,0.)
ptoDest=geom.Pos3d(1.,1.,0.)
r1=geom.Line3d(fromPoint,ptoDest)

svd1=geom.SlidingVectorsSystem3d()
ptoAplic=geom.Pos3d(1,1,0)
vectorDir=geom.Vector3d(0,0,1)
vec=geom.SlidingVector3d(ptoAplic,vectorDir)
svd1+=vec
ptoAplic=geom.Pos3d(-1,1,0)
vec=geom.SlidingVector3d(ptoAplic,vectorDir)
svd1+=vec
ptoAplic=geom.Pos3d(-1,-1,0)
vec=geom.SlidingVector3d(ptoAplic,vectorDir)
svd1+=vec
ptoAplic=geom.Pos3d(1,-1,0)
vec=geom.SlidingVector3d(ptoAplic,vectorDir)
svd1+=vec

Res=svd1.getResultant()
Mom=svd1.getMoment()
MomP=svd1.getMomentPos3d(pto)
MomR=svd1.getMomentLine3d(r1) #devuelve el valor con signo (en función del sentido de la recta), probablemente sea mejor ignorar el signo

##print("Res.x =", Res.x)
##print("Res.y =", Res.y)
##print("Res.z =", Res.z)
##print("Mom.x =", Mom.x)
##print("Mom.y =", Mom.y)
##print("Mom.z =", Mom.z)
##print("MomR =", MomR)
##print("MomP.x =", MomP.x)
##print("MomP.y =", MomP.y)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if Res.x == 0 and Res.y == 0 and Res.z == 4 and Mom.x == 0 and Mom.y == 0 and Mom.z == 0 and MomR == -4 and MomP.x == -4 and MomP.y == -4:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


