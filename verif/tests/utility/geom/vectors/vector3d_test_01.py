# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
vec1=geom.Vector3d(1,2,3)
xV=vec1.x
yV=vec1.y
zV=vec1.z

tp=vec1.tipo()

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if xV==1 and yV==2 and zV==3:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')












