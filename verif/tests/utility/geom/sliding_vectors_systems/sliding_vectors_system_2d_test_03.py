# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

O=geom.Pos2d(0,0)
A=geom.Pos2d(1,0)
V=geom.Vector2d(0,1)

# A: point where the resultant is applied.
# V: resultant of the sliding vector system.
# M: momoent of the sliding vectors system with respect to the A point
#    (zero in this case).
svs1= geom.SlidingVectorsSystem2d(A,V,0.0)
pto= svs1.getOrg()
momO= svs1.getMoment(O)

svs2= svs1.reduceTo(O)
momA= svs2.getMoment(A)
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

