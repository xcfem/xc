from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
o=geom.Pos3d(0,0,0)
p1=geom.Pos3d(1,0,0)
p2=geom.Pos3d(0,1,0)

plane=geom.Plane3d(o,p1,p2)
normal=plane.getNormal()
base1=plane.getBase1()
base2=plane.getBase2()
tp=plane.tipo()

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if normal.x==0 and normal.y==0 and normal.z==1:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


