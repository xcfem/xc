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

r1=geom.Line3d(geom.Pos3d(0.,0.,0.),geom.Pos3d(10.,10.,10.))

vdir= r1.getVDir()
tp=r1.tipo()

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if vdir.x==10. and vdir.y==10. and vdir.z==10.:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')















