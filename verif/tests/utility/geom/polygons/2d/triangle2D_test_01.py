# -*- coding: utf-8 -*-
''' Trivial test for Triangle2d class.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

triangle= geom.Triangle2d(geom.Pos2d(0,0), geom.Pos2d(1,0), geom.Pos2d(0,1))
areaTriangle= triangle.getArea()
polygonPerimeter= triangle.getPerimeter()
polygonCenterOfMass= triangle.getCenterOfMass()
polygonCenterOfMassX= polygonCenterOfMass.x
polygonCenterOfMassY= polygonCenterOfMass.y

ratio1= (areaTriangle-0.5)
refPerimeter= 2+math.sqrt(2)
ratio2= (polygonPerimeter-refPerimeter)/refPerimeter
ratio3= (polygonCenterOfMassX-1/3.0)*3
ratio4= (polygonCenterOfMassY-1/3)*3

err= 0.0
edges= triangle.getEdgeList()
for edge in edges:
    err+= (edge.getLength()-1)**2
err= math.sqrt(err)

# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
