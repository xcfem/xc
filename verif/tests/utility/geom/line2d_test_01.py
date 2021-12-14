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

r1=geom.Line2d(geom.Pos2d(0.,0.),geom.Pos2d(10.,0.))
r2=geom.Line2d(geom.Pos2d(0.,-2.),geom.Pos2d(10.,-2.))
r3=geom.Line2d(geom.Pos2d(0.,2.),geom.Pos2d(10.,2.))

a=r2.getParamA()
b=r2.getParamB()

ratio1=math.fabs(a)
ratio2=math.fabs(b+2)

a=r3.getParamA()
b=r3.getParamB()

ratio3=math.fabs(a)
ratio4=math.fabs(b-2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-15 and ratio2<1e-15 and ratio3<1e-15 and ratio4<1e-15 :
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')













