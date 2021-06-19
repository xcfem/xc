# -*- coding: utf-8 -*-

#Angle between two planes.
#Exaple 1.7 from the thesis «La teoría de bloque aplicada a la dinámica
#de rocas» Juan Carlos Ayes Zamudio

from __future__ import print_function
import xc_base
import geom
import math
import teoria_bloques

alpha1=math.radians(30)
beta1=math.radians(320)
p=geom.Pos3d(0,0,0)

plBuz1=teoria_bloques.computeDipPlane(alpha1,beta1,p)
P1=geom.HalfSpace3d(plBuz1)
v=geom.Vector3d(1,2,1)

delta= P1.getAngle(v)
deltaTeor=math.radians(32.349)

ratio1=math.fabs(deltaTeor-delta)/deltaTeor

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-4:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

