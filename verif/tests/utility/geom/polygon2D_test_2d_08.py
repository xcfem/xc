# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
pol1=geom.Polygon2d()
pol1.appendVertex(geom.Pos2d(0.,0.))
pol1.appendVertex(geom.Pos2d(4.,0.))
pol1.appendVertex(geom.Pos2d(4.,1.))
pol1.appendVertex(geom.Pos2d(0.,1.))

areasTrib=pol1.getTributaryAreas()

ratio1= areasTrib[0]-1
ratio2= areasTrib[1]-1
ratio3= areasTrib[2]-1
ratio4= areasTrib[3]-1

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
