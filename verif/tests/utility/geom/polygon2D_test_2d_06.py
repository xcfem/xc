# -*- coding: utf-8 -*-
from __future__ import print_function
import xc_base
import geom
import math

referencePerimeter=4+math.sqrt(2)

pol1=geom.Polygon2d()
pol1.appendVertex(geom.Pos2d(-1.,-1.))
pol1.appendVertex(geom.Pos2d(1.,-1.))
pol1.appendVertex(geom.Pos2d(1.,1.))
pol1.appendVertex(geom.Pos2d(-1.,1.))

pol2=geom.Polygon2d(pol1)
pol2.move(geom.Vector2d(1,1))

pol3=geom.Polygon2d()
pol3.appendVertex(geom.Pos2d(0.,-1.))
pol3.appendVertex(geom.Pos2d(1.,-1.))
pol3.appendVertex(geom.Pos2d(1.,2.))
pol3.appendVertex(geom.Pos2d(0.,2.))

list1=[pol1,pol2]
list1=geom.particiona(list1)
list1=geom.clip(list1,pol3)

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
