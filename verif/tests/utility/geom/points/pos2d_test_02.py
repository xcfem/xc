# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
pos= geom.Pos2d(0,0)

# get buffer polygon.
r= 1.0
numVertices= 8 # Eight sides.
pg= pos.getBufferPolygon(r, numVertices) # Eight sides.
# Compute area.
area= pg.getArea()
# Compute theoretical area.
refArea= 0.5*numVertices*r*r*math.sin(2*math.pi/numVertices)
# Compute relative error.
ratio= abs(area-refArea)/refArea

#print(area, refArea,ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-5):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
 
