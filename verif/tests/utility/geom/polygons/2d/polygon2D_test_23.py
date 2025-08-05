# -*- coding: utf-8 -*-
'''Test computation of principal axes of inertia for Polygon2d objects.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom

# Define trivial rectangle.
points= [geom.Pos2d(-0.5,-1), geom.Pos2d(0.5, -1), geom.Pos2d(0.5, 1), geom.Pos2d(-0.5, 1)]
plg2d= geom.Polygon2d(points)

principalAxes= plg2d.getPrincipalAxes()

org= principalAxes.Org
refOrg= geom.Pos2d(0,0)
ratio0= refOrg.dist(org)
I1= principalAxes.I1
I1ref= 1/12.0*1*2**3
ratio1= abs(I1-I1ref)/I1ref
I2= principalAxes.I2
I2ref= 1/12.0*2*1**3
ratio2= abs(I2-I2ref)/I2ref

axis1= principalAxes.getAxis1VDir()
refAxis1= geom.Vector2d(1, 0)
ratio3= (axis1-refAxis1).getModulus()
axis2= principalAxes.getAxis2VDir()
refAxis2= geom.Vector2d(0, 1)
ratio4= (axis2-refAxis2).getModulus()

'''
print('org= ', org)
print('ratio0= ', ratio0)
print('I1= ', I1, 'm4')
print('I1ref= ', I1ref, 'm4')
print('ratio1= ', ratio1)
print('I2= ', I2, 'm4')
print('I2ref= ', I2ref, 'm4')
print('ratio2= ', ratio2)
print('axis1= ', axis1)
print('refAxis1= ', refAxis1)
print('ratio3= ', ratio3)
print('axis2= ', axis2)
print('refAxis2= ', refAxis2)
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-10 and abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
