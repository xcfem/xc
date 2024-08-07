# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

posA= geom.Pos2d(1,2)

for i in range(0,10):
    key= chr(65+i)
    posA.setProp(key, i)

posB= geom.Pos3d(0,0,1)
posB.copyPropsFrom(posA)

bDict= posB.getDict()
refBDict= {'className': 'Pos3d', 'py_prop': {'A': 0, 'B': 1, 'C': 2, 'D': 3, 'E': 4, 'F': 5, 'G': 6, 'H': 7, 'I': 8, 'J': 9}, 'exts': True, 'x': 0.0, 'y': 0.0, 'z': 1.0}

'''
print(refBDict)
print(bDict)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (bDict==refBDict):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
