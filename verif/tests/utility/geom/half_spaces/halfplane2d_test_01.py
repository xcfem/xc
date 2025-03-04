# -*- coding: utf-8 -*-
'''Trivial half-plane test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

p0= geom.Pos2d(0,0)
p1= geom.Pos2d(1,0)
p2= geom.Pos2d(0,1)

ln= geom.Line2d(p1, p2)

hp= geom.HalfPlane2d(ln, p0)

testOK= hp.In(p0, 0.0)
testOK= testOK and not hp.In(geom.Pos2d(1,1), 0.0)

# print(testOK)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
