# -*- coding: utf-8 -*-
''' Check addition for sliding vectors operators.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

pA= geom.Pos2d(0.0,0.0); vA= geom.Vector2d(-1e3,-1e3)
pB= geom.Pos2d(1.0,0.0); vB= geom.Vector2d(-1e3, 1e3)

svs1= geom.SlidingVectorsSystem2d(geom.SlidingVector2d(pB, vB))
Msvs1= svs1.getMoment(pA)
ratio1= abs(Msvs1-1000)/1000

svs2= geom.SlidingVector2d(pA, vA)+geom.SlidingVector2d(pB, vB)
Msvs2= svs2.getMoment(pA)
ratio2= abs(Msvs2-1000)/1000

# Compute an application point for the sliding vector system
zml= svs2.zeroMomentLine()
center= geom.Pos2d(0.5,0.0)
vLine= geom.Line2d(center, geom.Vector2d(0.0,100.0))
appPt= vLine.getIntersection(zml)[0]
MA= svs2.getMoment(pA)
ratio3= abs(MA-1000)/1000
ratio4= math.sqrt((appPt.x-0.5)**2+(appPt.y-0.5)**2)


'''
print(appPt, MA)
print(ratio1)
print(ratio2)
print(ratio3)
print(ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) and (ratio2<1e-15) and (ratio3<1e-15) and (ratio4<1e-15)):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

