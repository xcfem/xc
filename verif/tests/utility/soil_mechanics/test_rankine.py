# -*- coding: utf-8 -*-

from __future__ import print_function
from geotechnics import earth_pressure
import math

Kp= earth_pressure.RankineSoil(math.pi/6.0).Kp()

#print('Kp= ', Kp)

ratio1= abs(Kp-3)/3.0

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
