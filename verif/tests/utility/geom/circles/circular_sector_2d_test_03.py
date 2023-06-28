# -*- coding: utf-8 -*-
''' Circular sector getMidPoint method test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from misc_utils import log_messages as lmsg

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45

pA= geom.Pos2d(R,0.0)
pB= geom.Pos2d((R*cos45),(R*sin45))
pC= geom.Pos2d(0.0,R)
arc1= geom.CircularSector2d(pA, pB, pC)
arc2= geom.CircularSector2d(pC, pB, pA)

pD= pA
pE= pC
pF= geom.Pos2d(-R,0)
arc3= geom.CircularSector2d(pD, pE, pF)
arc4= geom.CircularSector2d(pF, pE, pD)

# Check mid-points and end points.
firstPoint1= arc1.getFromPoint()
midPoint1= arc1.getMidPoint()
err1= math.sqrt(pB.dist2(midPoint1)+pA.dist2(firstPoint1))
firstPoint2= arc2.getFromPoint()
midPoint2= arc2.getMidPoint()
err2= math.sqrt(pB.dist(midPoint2)+pC.dist2(firstPoint2))
firstPoint3= arc3.getFromPoint()
midPoint3= arc3.getMidPoint()
err3= math.sqrt(pE.dist(midPoint3)+pD.dist2(firstPoint3))
firstPoint3= arc3.getFromPoint()
midPoint3= arc3.getMidPoint()
err3= math.sqrt(pE.dist(midPoint3)+pD.dist2(firstPoint3))
firstPoint4= arc4.getFromPoint()
midPoint4= arc4.getMidPoint()
err4= math.sqrt(pE.dist(midPoint4)+pF.dist2(firstPoint4))

'''
print(pA, firstPoint1, err1)
print(pC, firstPoint2, err2)
print(pD, firstPoint3, err3)
print(pF, firstPoint3, err3)
'''

import os
fname= os.path.basename(__file__)
if abs(err1)<1e-6 and abs(err2)<1e-6 and abs(err3)<1e-6:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
