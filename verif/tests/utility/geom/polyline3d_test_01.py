# -*- coding: utf-8 -*-
#Home made test. Verification of Douglas Peucker algorithm implementation.

from __future__ import print_function
import xc_base
import geom
import math

polA=geom.Polyline3d()

polA.appendVertex(geom.Pos3d(0,0,0)) #1
polA.appendVertex(geom.Pos3d(0.001,0.5,0.09)) #2 (to erase)
polA.appendVertex(geom.Pos3d(0,1,0)) #3
polA.appendVertex(geom.Pos3d(0.002,1.001,0.5)) #4 (to erase)
polA.appendVertex(geom.Pos3d(0,1,1)) #5
polA.appendVertex(geom.Pos3d(0,2,1)) #6
polA.appendVertex(geom.Pos3d(0,2,2)) #7
polA.appendVertex(geom.Pos3d(0,1,2)) #8
polA.appendVertex(geom.Pos3d(0,1,1)) #9
polA.appendVertex(geom.Pos3d(0,0,1)) #10

polB= geom.Polyline3d(polA)

polB.appendVertex(geom.Pos3d(0,0,0)) #11 CAN BE CLOSED.

nv0A= polA.getNumVertices()
polA.simplify(0.1)
nv1A= polA.getNumVertices()

nv0B= polB.getNumVertices()
polB.simplify(0.1)
nv1B= polB.getNumVertices()

nv0ATeor= 10
nv1ATeor= nv0ATeor-2
ratio0=(nv0A-nv0ATeor)/nv0ATeor
ratio1=(nv1A-nv1ATeor)/nv1ATeor
nv0BTeor= nv0ATeor+1
nv1BTeor= nv0BTeor-2
ratio3=(nv0B-nv0BTeor)/nv0BTeor
ratio4=(nv1B-nv1BTeor)/nv1BTeor

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio0)<1e-10 and math.fabs(ratio1)<1e-10 and math.fabs(ratio3)<1e-10 and math.fabs(ratio4)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


