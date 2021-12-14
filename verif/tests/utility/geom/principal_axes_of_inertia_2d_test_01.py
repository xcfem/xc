# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
axes= geom.PrincipalAxesOfInertia2D(geom.Pos2d(1,2),1.0,1.0,0.0)

i1= axes.I1
i2= axes.I2

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(i1==1. and i2==1.):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
