# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

o= geom.Pos2d(0,0)
v= geom.Vector2d(0, 1)
# Reflection on the y-axis.
yAxisReflection2d= geom.Reflection2d(o, v)
p= geom.Pos2d(1, 1)
pp= yAxisReflection2d.getTransformed(p)
# Reflection on the x-axis.
xAxis= geom.Line2d(o, geom.Vector2d(1, 0))
xAxisReflection2d= geom.Reflection2d(xAxis)
q= geom.Pos2d(1, -1)
qq= xAxisReflection2d.getTransformed(q)

ratio1= abs(p.x+pp.x)
ratio2= abs(p.y-pp.y)
ratio3= abs(q.x-qq.x)
ratio4= abs(q.y+qq.y)

'''
print("p= ", p)
print("pp= ", pp)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("q= ", q)
print("qq= ", qq)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and  abs(ratio3)<1e-12 and abs(ratio4)<1e-12:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')









