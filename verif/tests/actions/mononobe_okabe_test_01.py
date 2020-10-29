# -*- coding: utf-8 -*-
from __future__ import print_function

'''Verification of Mononobe-Okabe routines.
   Home made test.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.earth_pressure import earth_pressure

zGround= 0.0 #m z coordinate of the ground surface.
gammaSoil= 21e3 #N/m3 unit weight of soil (backfill)
H= 4 #m height of the structure.
kh= 0.086 #seismic coefficient of horizontal acceleration.
kv= kh/2.0 #seismic coefficient of vertical acceleration.
psi= math.radians(90) #back face inclination of the structure (<= PI/2)
phi= math.radians(30.0) #angle of internal friction of soil.
delta_ad= 0.0 #angle of friction soil - structure.
beta= 0.0 #slope inclination of backfill.
Kas= 1/3.0 #static earth pressure coefficient 

mononobeOkabe= earth_pressure.MononobeOkabePressureDistribution(zGround, gammaSoil, H, kv, kh, psi, phi, delta_ad, beta, Kas)

overPressure= mononobeOkabe.overpressure_dry
sigmaMax= mononobeOkabe.getPressure(0.0)
sigma= mononobeOkabe.getPressure(-H/2.0)
sigmaMin= mononobeOkabe.getPressure(-H)

ratio1= (overPressure-11.3919636706e3)/11.3919636706e3
ratio2= (sigmaMax-5.69598183531e3)/5.69598183531e3
ratio3= (sigma-sigmaMax/2.0)/(sigmaMax/2.0)
ratio4= sigmaMin

'''
print('overPressure= ', overPressure/1e3, ' kN/m')
print('ratio1= ', ratio1 )
print('sigmaMax= ', sigmaMax/1e3, ' kN/m2')
print('ratio2= ', ratio2 )
print('sigma= ', sigma/1e3, ' kN/m2')
print('ratio3= ', ratio3 )
print('sigmaMin= ', sigmaMin/1e3, ' kN/m2')
print('ratio4= ', ratio4)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and abs(ratio3)<1e-6and abs(ratio4)<1e-6:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
