# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom

posA= geom.Pos2d(1,2)

for i in range(0,10):
    key= chr(65+i)
    posA.setProp(key, i)

posB= geom.Pos3d(0,0,1)
posB.copyPropsFrom(posA)

bDict= posB.getDict()
refBDict= {'py_propA': 0, 'py_propB': 1, 'py_propC': 2, 'py_propD': 3, 'py_propE': 4, 'py_propF': 5, 'py_propG': 6, 'py_propH': 7, 'py_propI': 8, 'py_propJ': 9, 'exts': True, 'x': 0.0, 'y': 0.0, 'z': 1.0}

# print(refBDict)
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (bDict==refBDict):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
