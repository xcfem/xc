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

pto=geom.Pos3d(0,0,0)
vectorDir=geom.Vector3d(1,2,3)
vec1=geom.SlidingVector3d(pto,vectorDir)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if vec1.x==1 and vec1.y==2 and vec1.z==3:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


