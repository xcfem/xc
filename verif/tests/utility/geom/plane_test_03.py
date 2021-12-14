# -*- coding: utf-8 -*-
'''Intersection of plane with a line.
vExemple 1.2 of the thesis «La teoría de bloque aplicada a la dinámica
 de rocas» de Juan Carlos Ayes Zamudio.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

A=2.
B=3.
C=1.
D=4.

ITeor=geom.Pos3d(3,-1,1)
P=geom.Plane3d(geom.GeneralEquationOfPlane(2,3,1,-4))
pto=geom.Pos3d(1,1,-2)
vDir=geom.Dir3d(2,-2,3)

r=geom.Line3d(pto,vDir)
dr=ITeor.dist(r)
I=P.getIntersection(r)
d=I.dist(P)

ratio1=ITeor.dist(I)
ratio2=math.fabs(dr)
ratio3=math.fabs(d)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-4 and math.fabs(ratio2)<1e-4 and math.fabs(ratio3)<1e-4:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

