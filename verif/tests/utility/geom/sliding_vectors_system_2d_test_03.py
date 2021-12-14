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

O=geom.Pos2d(0,0)
A=geom.Pos2d(1,0)
V=geom.Vector2d(0,1)

svs1= geom.SlidingVectorsSystem2d(A,V,0.0)
pto= svs1.getOrg()
momO= svs1.getMomentPos2d(O)

svs2= svs1.reduceTo(O)
momA= svs2.getMomentPos2d(A)
ratio1= momA

'''
print(pto)
print(momO)
print(momA.getModulus())
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-12:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

