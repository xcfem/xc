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
pol2=geom.Polygon2d()
pol3=geom.Polygon2d()

pList= geom.polygon_2D_list()
pList.append(pol1)
pList.append(pol2)
pList.append(pol3)

# for p in pList:
#   print(p.getArea())

ratio= len(pList)-3

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio)<1e-10):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
