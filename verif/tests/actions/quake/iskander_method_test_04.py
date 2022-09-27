# -*- coding: utf-8 -*-
'''Verification of Mononobe-Okabe routines.
   Home made test.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.earth_pressure import earth_pressure

zGround= 0.0 #m z coordinate of the ground surface.
gammaSoil= 21e3 #N/m3 unit weight of soil (backfill)
H= 4 # m height of the structure.
kh= 0.086 # seismic coefficient of horizontal acceleration.
kv= kh/2.0 # seismic coefficient of vertical acceleration.
psi= math.radians(90) # back face inclination of the structure (<= PI/2)
phi= math.radians(30.0) # angle of internal friction of soil.
beta= 0.0 # slope inclination of backfill.
c= 0.0 # cohesion

earthPressure= earth_pressure.IskanderPressureDistribution(zGround= zGround,gammaSoil= gammaSoil, H= H, kv= kv, kh= kh, psi= psi, phi= phi, beta= beta, c= c)

sigmaMin= earthPressure.getPressure(0.0)
sigma= earthPressure.getPressure(-H/2.0)
sigmaMax= earthPressure.getPressure(-H)

refSigmaMax= 2.401439275521134e3
ratio1= (sigmaMax-refSigmaMax)/refSigmaMax
ratio2= (sigma-sigmaMax/2.0)/(sigmaMax/2.0)
ratio3= sigmaMin

'''
print('sigmaMax= ', sigmaMax/1e3, ' kN/m2')
print('refSigmaMax= ', refSigmaMax/1e3, ' kN/m2')
print('ratio1= ', ratio1 )
print('sigma= ', sigma/1e3, ' kN/m2')
print('ratio2= ', ratio2 )
print('sigmaMin= ', sigmaMin/1e3, ' kN/m2')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and abs(ratio3)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
