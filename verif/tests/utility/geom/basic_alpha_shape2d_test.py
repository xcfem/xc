# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom

pts= [geom.Pos2d(0,0), geom.Pos2d(1,0), geom.Pos2d(0.5,1)]
ref_plg= geom.Polygon2d(pts)
pts.extend([geom.Pos2d(0.5,0.5), geom.Pos2d(0.5,0.75)])

points= geom.PolyPos2d()
for p in pts:
  points.append(p)

plg= geom.get_basic_alpha_shape2d(points)

areaPlg= plg.getArea()
areaRef= ref_plg.getArea()
ratio1= abs(areaPlg-areaRef)/areaRef

'''
print('areaPlg= ', areaPlg, areaRef)
print('ratio1= ', ratio1)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

