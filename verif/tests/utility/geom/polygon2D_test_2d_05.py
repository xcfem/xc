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

referencePerimeter=6+math.sqrt(2)

pol1=geom.Polygon2d()
pol1.appendVertex(geom.Pos2d(-1.,-1.))
pol1.appendVertex(geom.Pos2d(1.,-1.))
pol1.appendVertex(geom.Pos2d(1.,1.))
pol1.appendVertex(geom.Pos2d(-1.,1.))

pol2=geom.Polygon2d(pol1)
pol2.move(geom.Vector2d(1,1))

list1=[pol1,pol2]
list1=geom.particiona(list1)

p1=list1[0]
ratio1=(p1.getPerimeter()-referencePerimeter)/referencePerimeter

p2=list1[1]
ratio2=(p2.getPerimeter()-referencePerimeter)/referencePerimeter

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

