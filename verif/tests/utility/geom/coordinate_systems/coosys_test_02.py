# -*- coding: utf-8 -*-
''' Check the constructor of CooSysRect2d2d that receives two points.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
o= geom.Pos2d(0,0)
p= geom.Pos2d(0,-1.65)
sc= geom.CooSysRect2d2d(o, p)
ratio= (sc.getJVector()-geom.Vector2d(1,0)).getModulus()

# print('I= ', sc.getIVector())
# print('J= '. sc.getJVector())
# print("ratio= ", ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')










