# -*- coding: utf-8 -*-
''' Check the constructor of CooSysRect3d3d that receives two points.

This tests was made to fix an error in the parallel(Dir3d, Dir3d) function.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
o= geom.Pos3d(0,0,0)
p= geom.Pos3d(0,0,-1.65)
sc= geom.CooSysRect3d3d(o, p)
ratio= (sc.getKVector()-geom.Vector3d(0,1,0)).getModulus()

# print('K= ', sc.getKVector())
# print("ratio= ", ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')










