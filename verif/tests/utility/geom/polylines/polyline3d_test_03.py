# -*- coding: utf-8 -*-
'''Home made test. Verification of split method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math
vertices= [  geom.Pos3d(0,0,0), #1
  geom.Pos3d(1,0,0), #2
  geom.Pos3d(1,2,0), #3
  ]

polA= geom.Polyline3d(vertices)

# Split at vertex.
resultVertex= polA.split(geom.Pos3d(1,0,0))
lengthA= resultVertex[0].getLength()
ratio0= abs(lengthA-1)
lengthB= resultVertex[1].getLength()
ratio1= abs(lengthB-2)/2

# Split at segment.
resultVertex= polA.split(geom.Pos3d(1,1,0))
lengthC= resultVertex[0].getLength()
ratio2= abs(lengthC-2)/2
lengthD= resultVertex[1].getLength()
ratio3= abs(lengthD-1)

'''
print('lengthA= ', lengthA)
print('ratio0= ', ratio0)
print('lengthB= ', lengthB)
print('ratio1= ', ratio1)
print('lengthC= ', lengthC)
print('ratio2= ', ratio2)
print('lengthD= ', lengthD)
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-10 and abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


