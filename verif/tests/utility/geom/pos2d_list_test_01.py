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

ptsPrb=geom.ListPos2d()
ptsPrb.appendPoint(geom.Pos2d(0,0))
ptsPrb.appendPoint(geom.Pos2d(2,0))
ptsPrb.appendPoint(geom.Pos2d(2,2))
ptsPrb.appendPoint(geom.Pos2d(0,2))
centerOfMassX=ptsPrb.getCenterOfMass().x
centerOfMassY=ptsPrb.getCenterOfMass().y
area=ptsPrb.getArea()

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(centerOfMassX-1.0)<1e-15 and math.fabs(centerOfMassY-1.0)<1e-15 and math.fabs(area)<1e-15:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

