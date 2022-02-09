# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

fromPoint=geom.Pos3d(0.5,0,0)
ptoDest=geom.Pos3d(0.5,0,100)
r1=geom.Line3d(fromPoint,ptoDest)
vdir=r1.getVDir()
tp=r1.tipo()

pto_aplic=geom.Pos3d(5,0,0)
vectorDir=geom.Vector3d(0,2,0)
vec1=geom.SlidingVector3d(pto_aplic,vectorDir)
pto=vec1.getOrg()
brazo=pto.dist(r1)
fuerza=vec1.y
m=vec1.getMomentLine3d(r1)

mTeor=fuerza*brazo
ratio1=(m-mTeor)/mTeor

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if m==mTeor:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

