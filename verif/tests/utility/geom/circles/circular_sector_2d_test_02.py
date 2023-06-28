# -*- coding: utf-8 -*-
''' Circular sector offset method test.'''

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

vaultExt= geom.CircularSector2d(p0, p1, p2) # define circular sector.
vaultInt= vaultExt.offset(-0.25) # compute offset.
# Check some values.
pA= vaultInt.getFromPoint()
dA= pA.dist(p0)
pB= vaultInt.getMidPoint()
dB= pB.dist(p1)
pC= vaultInt.getToPoint()
dC= pC.dist(p2)
err= math.sqrt((dA-0.25)**2+(dB-0.25)**2+(dC-0.25)**2)

'''
print('dA= ', dA)
print('dB= ', dB)
print('dC= ', dC)
print('err= ', err)
'''

import os
fname= os.path.basename(__file__)
if abs(err)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
