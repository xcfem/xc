# -*- coding: utf-8 -*-
'''Trivial test of earth pressure due to an horizontal load on the backfill
   surface.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from actions.earth_pressure import earth_pressure
from misc_utils import log_messages as lmsg

# Geometry
H= 10 # Stem height (m)

# Define earth pressure.
# phi: angle of internal friction of the soil (rad)
# qLoad: surcharge load (force per unit length)
# zLoad: global Z coordinate where the line load acts
# distWall: minimal horizontal distance between the wall and
#           the area where the surcharge load acts (e.g.: a foundation)
# widthLoadArea: width (perperdicular to wall) of the area on
#                which the horizontal load acts (e.g.: a foundation). 
# lengthLoadArea: width (parallel to wall) of the area on
#                 which the horizontal load acts (e.g.: a foundation). We can
#                 take lengthLoadArea=1 (default) for a continous load  
# horDistrAngle: angle to distribute load in the direction of
#                the lengthLoadArea (defaults to 0 to apply all the load to a
#                length of wall =lengthLoadArea, which would be the case of a
#                continous load)
phi= math.radians(25)
alphaClassif= 1.0 # temporary structure;
qLoad= alphaClassif*100e3 # Nosing load.
zLoad= H-0.95
loadedAreaWidth= 3.0
loadedAreaLength= 1.0
pressure= earth_pressure.HorizontalLoadOnBackfill(phi= phi, qLoad= qLoad, zLoad= zLoad, distWall= 2.56, loadedAreaWidth= loadedAreaWidth,  loadedAreaLength= loadedAreaLength)

zMaxPressure= pressure.getZMaxPressure()
ratio0= abs(zMaxPressure-7.8563)/7.8563
zMinPressure= pressure.getZMinPressure()
ratio1= abs(zMinPressure-.3225)/.3225
maxPressure= pressure.getMaxPressure()
ratio2= abs(maxPressure-26547.4)/26547.4

zGround= H-0.5
virtualBack= geom.Segment2d(geom.Pos2d(0,0), geom.Pos2d(0, zGround))
R= -pressure.getForces2D(virtualBack, numDiv= 10, beta= 0.0).getResultant()[0]
ratio3= abs(R-qLoad)/qLoad

'''
print(zMaxPressure, ratio0)
print(zMinPressure, ratio1)
print(maxPressure, ratio2)
print(R, ratio3)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio0)<1e-4 and abs(ratio1)<1e-3 and abs(ratio2)<1e-4 and abs(ratio3)<1e-4:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
 
