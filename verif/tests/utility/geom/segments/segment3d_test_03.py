# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

A= geom.Pos3d(0,0,0)
B= geom.Pos3d(0,0,10)

a= geom.Segment3d(A,B)

P= a.getPoint(0.5*a.getLength())
refP= geom.Pos3d(0,0,5)
err= (P-refP).getModulus()

# print(P, refP, err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(err)<1e-10):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
