# -*- coding: utf-8 -*-
''' Test the computation of the Frenet trihedron along a circular arc contained
    in the XZ plane.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

pth= geom.Polyline3d([geom.Pos3d(0,0,0), geom.Pos3d(1,0,0), geom.Pos3d(1,0,1)])

ft= geom.FrenetTrihedron(pth)

halfSqrt2= math.sqrt(2)/2.0
t0= ft.getTangent(0.0)
t0Ref= geom.Vector3d(halfSqrt2, 0, -halfSqrt2)
t1= ft.getTangent(1.0)
t1Ref= geom.Vector3d(halfSqrt2, 0, halfSqrt2)
t2= ft.getTangent(2.0)
t2Ref= geom.Vector3d(-halfSqrt2, 0, halfSqrt2)

n0= ft.getNormal(0.0)
n0Ref= geom.Vector3d(halfSqrt2, 0, halfSqrt2)
n1= ft.getNormal(1.0)
n1Ref= geom.Vector3d(-halfSqrt2, 0, halfSqrt2)
n2= ft.getNormal(2.0)
n2Ref= geom.Vector3d(-halfSqrt2, 0, -halfSqrt2)

bRef= geom.Vector3d(0, -1, 0)
b0= ft.getBinormal(0.0)
b1= ft.getBinormal(1.0)
b2= ft.getBinormal(2.0)

err= (t0-t0Ref).getModulus2()+(t1-t1Ref).getModulus2()+(t2-t2Ref).getModulus2()
err+= (n0-n0Ref).getModulus2()+(n1-n1Ref).getModulus2()+(n2-n2Ref).getModulus2()
err+= (b0-bRef).getModulus2()+(b1-bRef).getModulus2()+(b2-bRef).getModulus2()

err= math.sqrt(err)

'''
print('t0= ', t0)
print('t1= ', t1)
print('t2= ', t2)
print('\nn0= ', n0)
print('n1= ', n1)
print('n2= ', n2)
print('\nb0= ', b0)
print('b1= ', b1)
print('b2= ', b2)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')











