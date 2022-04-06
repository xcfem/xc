# -*- coding: utf-8 -*-
''' Direction vector of the intersection of two planes.
 Example 1.3 of the thesis «La teoría de bloque aplicada a la dinámica
 de rocas» by Juan Carlos Ayes Zamudio.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math
import teoria_bloques

alpha1=math.radians(20)
beta1=math.radians(280)
alpha2=math.radians(60)
beta2=math.radians(150)

O=geom.Pos3d(0,0,0)

P1=teoria_bloques.computeDipPlane(alpha1,beta1,O)
P2=teoria_bloques.computeDipPlane(alpha2,beta2,O)

n1=P1.getNormal()
n2=P2.getNormal()

I12=n1.cross(n2)

I12Teor=geom.Vector3d(0.7345,0.5753,0.2269)

ratio1=(I12-I12Teor).getModulus()

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-4:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

#Terminar!!!
