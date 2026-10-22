# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
points= [geom.Pos2d(-0.5,-0.2), geom.Pos2d(0.5, -0.2), geom.Pos2d(0.5, 0.2), geom.Pos2d(-0.5, 0.2)]
plg2d= geom.Polygon2d(points)

principalAxes= plg2d.getPrincipalAxes()

majorAxisLine= principalAxes.getAxis1Line()
minorAxisLine= principalAxes.getAxis2Line()

org= geom.Pos2d(0, 0)
majorAxisDir= majorAxisLine.getVDir().normalized()
minorAxisDir= minorAxisLine.getVDir().normalized()

# Check results.
err= majorAxisLine.dist2(org)
err+= minorAxisLine.dist2(org)
err+= (majorAxisDir-geom.Vector2d(0,1)).getModulus2()
err+= (minorAxisDir-geom.Vector2d(-1,0)).getModulus2()

err= math.sqrt(err)

'''
print(minorAxisLine)
print(majorAxisDir)
print(minorAxisDir)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-12:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
