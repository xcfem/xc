# -*- coding: utf-8 -*-
''' Check k_janssen function.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc
from geotechnics import earth_pressure
from scipy.integrate import trapezoid

wallHeight= 7.5
backfillWidth= wallHeight/3.0

# Soil model
phi= math.radians(30)
rankineSoil= earth_pressure.RankineSoil(phi, rho= 2000.0)


# Compute z values along the wall depth.
incZ= 0.25
n= int(wallHeight/incZ)
zi= list()
for i in range(0, n):
    zi.append(-i*0.25)
zi.append(-wallHeight)


# Compute Janssen's earth pressure coefficient values.
ki_janssen= list()
k0= rankineSoil.K0Jaky()
d= 2*phi/3.0 # friction angle between soil an back of retaining wall (radians).
for z in zi:
    k_janssen= earth_pressure.k_janssen(k0,d= d, B= backfillWidth,z= -z)
    ki_janssen.append(k_janssen)

# Compute vertical stresses.
sigma_vi= [-rankineSoil.gamma()*z for z in zi]
sigma_hi_janssen= list()
sigma_hi_jaky= list()
for k, sigma_v in zip(ki_janssen, sigma_vi):
    sigma_hi_janssen.append(k*sigma_v)
    sigma_hi_jaky.append(k0*sigma_v)
    
R_janssen= -trapezoid(zi, sigma_hi_janssen)
R_jaky= -trapezoid(zi, sigma_hi_jaky)
ratio1= abs(R_janssen-137.9124010434513e3)/137.9124010434513e3
ratio2= abs(R_jaky-275.81203125e3)/275.81203125e3
'''
print(zi)
print(ki_janssen)
print(sigma_vi)
print(sigma_hi_janssen)
print('R according to janssen method: ', R_janssen/1e3, 'kN')
print('ratio1= ', ratio1)
print('R at rest: ', R_jaky/1e3, 'kN')
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-8) and (ratio2<1e-8)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# import matplotlib.pyplot as plt
# ## Units.
# sigma_hi_janssen= [x * 1e-3 for x in sigma_hi_janssen]
# sigma_hi_jaky= [x * 1e-3 for x in sigma_hi_jaky]
# plt.title("Horizontal earth pressure.")
# plt.xlabel('Earth pressure (kPa)')
# plt.ylabel('Depth (m)')
# plt.plot(sigma_hi_janssen, zi, 'o', label="Janssen")
# plt.plot(sigma_hi_jaky, zi, '-', label="Jaky")
# plt.legend(loc="upper right")
# plt.grid()
# plt.show()
