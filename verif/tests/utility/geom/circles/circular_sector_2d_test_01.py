# -*- coding: utf-8 -*-
''' Circular sector test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from misc_utils import log_messages as lmsg

vaultExtDiameter= 1.5 # upper vault external diameter.
vaultExtRadius= vaultExtDiameter/2.0 # upper vault external radius.

p0= geom.Pos2d(vaultExtRadius,-vaultExtRadius)
p1= geom.Pos2d(0.0,0.0)
p2= geom.Pos2d(-vaultExtRadius,-vaultExtRadius)

vaultExt= geom.CircularSector2d(p0, p1, p2)
center= vaultExt.getCenter()
th0= vaultExt.getAngle(p0)
th1= vaultExt.getAngle(p1)
th2= vaultExt.getAngle(p2)

# Check values.
err0= center.dist2(geom.Pos2d(0.0,-.75))
err1= math.sqrt((th0)**2+(th1-math.pi/2.0)**2+(th2-math.pi)**2)

'''
print('vault center: ', center)
print('err0= ', err0)
print('th0= ', math.degrees(th0),'º')
print('th1= ', math.degrees(th1),'º')
print('th2= ', math.degrees(th2),'º')
print('err1= ', err1)
'''

import os
fname= os.path.basename(__file__)
if abs(err0)<1e-10 and abs(err1)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
