# -*- coding: utf-8 -*-
''' Exemple 10.2 of the book "Principles of Foundation Engineering" 
    from Braja M. Das. Seventh Edition.
'''

from __future__ import division
import math
from actions.earth_pressure import earth_pressure

phi= math.radians(35)
gammaSoil= 17e3 #N/m3
zGround= 0.0
zWater= -1e6
gammaWater= 10e3 #N/m3
H= 9 #m

peckPressureEnvelope= earth_pressure.PeckPressureEnvelope(phi , zGround, gammaSoil, zWater, gammaWater, H)


pressure= peckPressureEnvelope.getPressure(-0.5)

pressureTeor= 0.65*17e3*9*0.27099005412
ratio1= abs(pressure-pressureTeor)/pressureTeor

'''
print 'Ka= ', peckPressureEnvelope.K
print 'pressure= ', pressure/1e3, ' kN/m2'
print 'ratio1= ', ratio1
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
