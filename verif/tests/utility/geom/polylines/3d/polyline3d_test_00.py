# -*- coding: utf-8 -*-
''' Test the getTangentVectorAtVertices method for the simplest case (only two
    vertices).'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

pth= geom.Polyline3d([geom.Pos3d(0,0,0), geom.Pos3d(1,0,0)])

tangentVectorAtVertices= pth.getTangentVectorAtVertices()
curvatureVectorAtVertices=  pth.getCurvatureVectorAtVertices()
normalVectorAtVertices=  pth.getNormalVectorAtVertices()

t0= tangentVectorAtVertices[0]
t0Ref= geom.Vector3d(1,0,0)
t1= tangentVectorAtVertices[1]
t1Ref= t0Ref

c0= curvatureVectorAtVertices[0]
c0Ref= geom.Vector3d(0,1,0)
c1= curvatureVectorAtVertices[1]
c1Ref= c0Ref

n0= normalVectorAtVertices[0]
n0Ref= geom.Vector3d(0,0,1)
n1= normalVectorAtVertices[1]
n1Ref= n0Ref



err= (t0-t0Ref).getModulus2()+(t1-t1Ref).getModulus2()
err+= (c0-c0Ref).getModulus2()+(c1-c1Ref).getModulus2()
err+= (n0-n0Ref).getModulus2()+(n1-n1Ref).getModulus2()
err= math.sqrt(err)

'''
print(tangentVectorAtVertices)
print(curvatureVectorAtVertices)
print(normalVectorAtVertices)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
