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

circ1=geom.Circle2d(geom.Pos2d(0.0,0.0),1.0)

areaCirc=circ1.getArea()
circlePerimeter= circ1.getPerimeter()
circleCenterOfMassX= circ1.getCenterOfMass().x
circleCenterOfMassY= circ1.getCenterOfMass().y
IxCirc=circ1.getIx()
IyCirc=circ1.getIy()
PxyCirc=circ1.getPxy()

ratio1=areaCirc-math.pi
ratio2=(circlePerimeter-2*math.pi)/2*math.pi
ratio3=circleCenterOfMassX
ratio4=circleCenterOfMassY
ratio5=(IxCirc-(math.pi/4.))/(math.pi/4.)
ratio6=(IyCirc-(math.pi/4.))/(math.pi/4.)
ratio7=PxyCirc

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-10 and math.fabs(ratio2)<1e-10 and math.fabs(ratio3)<1e-10 and math.fabs(ratio4)<1e-10  and math.fabs(ratio5)<1e-10 and math.fabs(ratio6)<1e-10 and math.fabs(ratio7)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')











