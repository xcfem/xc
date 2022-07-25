# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from geotechnics import earth_pressure
import math

phi= math.pi/6.0
rankineSoil= earth_pressure.RankineSoil(phi)

Kp= rankineSoil.Kp()
ratio1= abs(Kp-3)/3.0

Ka= rankineSoil.Ka()
phiFromKa= earth_pressure.phi_rankine_from_active_coefficient(b= 0.0, ka= Ka)
ratio2= abs(phiFromKa-phi)/phi


'''
print('Kp= ', Kp)
print(phiFromKa)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and abs(ratio2<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
