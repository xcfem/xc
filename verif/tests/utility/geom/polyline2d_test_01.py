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

pol1=geom.Polyline2d()
pol2=geom.Polyline2d()

pol1.appendVertex(geom.Pos2d(0,0))
pol1.appendVertex(geom.Pos2d(1,0))
pol1.appendVertex(geom.Pos2d(1,1))
pol1.appendVertex(geom.Pos2d(0,1))

longPol1=pol1.getLength()
pol2=pol1.offset(-0.25)
longPol2=pol2.getLength()

ratio1=(longPol1-3)/3.
ratio2=(longPol2-2)/2.

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-10 and math.fabs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


