# -*- coding: utf-8 -*-

#Projection of a vector onto a plane.
#Example 1.13 of the thesis «La teoría de bloque aplicada a la dinámica
#de rocas» by Juan Carlos Ayes Zamudio

from __future__ import print_function
import xc_base
import geom
import math
import teoria_bloques

alpha1=math.radians(20)
beta1=math.radians(280)
alpha2=math.radians(60)
beta2=math.radians(150)

p=geom.Pos3d(0,0,0)
R=geom.Vector3d(0,-1,1)

plBuz1=teoria_bloques.computeDipPlane(alpha1,beta1,p)
P1=geom.HalfSpace3d(plBuz1)

plBuz2=teoria_bloques.computeDipPlane(alpha2,beta2,p)
P2=geom.HalfSpace3d(plBuz2)

r=P1.getLima(P2)

s=r.getProj(R)
s=s.normalized()
sTeor=geom.Vector3d(-0.7650,-0.5992,-0.2363)
ratio1= sTeor-s
ratio1=ratio1.getModulus()

n1=P1.getNormalExterior()
n2=P2.getNormalExterior()
n3=n1.cross(n2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-4:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
