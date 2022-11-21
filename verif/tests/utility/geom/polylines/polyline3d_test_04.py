# -*- coding: utf-8 -*-
'''Home made test. Verification of getChunk method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

# Polyline:
#                3          4
#                +----------+
#                |          |
#                |          |
#                |          |
#                |          |
#     +----------+          +-----------+
#     1          2          5           6
#

vertices= [  geom.Pos3d(0,0,0), #1
  geom.Pos3d(0,1,0), #2
  geom.Pos3d(0,1,1), #3
  geom.Pos3d(0,2,1), #4
  geom.Pos3d(0,2,0), #5
  geom.Pos3d(0,3,0), #5
  ]

polA= geom.Polyline3d(vertices)

cutPoint= geom.Pos3d(0,1.5,1)

resultA= polA.getLeftChunk(cutPoint, 1e-3)
ratio1= (resultA.getLength()-2.5)/2.5
resultB= polA.getRightChunk(cutPoint, 1e-3)
ratio2= (resultB.getLength()-2.5)/2.5


'''
print(resultA, resultA.getLength(), ratio1)
print(resultB, resultB.getLength(), ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


