# -*- coding: utf-8 -*-
'''Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom

org= geom.Pos3d(0,0,0)

plg= geom.Polygon3d([org, geom.Pos3d(1,0,0), geom.Pos3d(0,1,0)])
distalEdgeIndex= plg.getIndexOfDistalEdge(org)
distalEdge= plg.getEdge(distalEdgeIndex)

d= distalEdge.dist(org)
dRef= math.sqrt(2.0)/2.0
ratio= abs(d-dRef)/dRef

'''
print('d= ', d)
print('dRef= ', dRef)
print('ratio= ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio<1e-12):
    print("test "+fname+": ok.")
else:
    print("test "+fname+": ERROR.")
