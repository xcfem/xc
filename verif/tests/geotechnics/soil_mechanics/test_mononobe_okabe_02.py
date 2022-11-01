# -*- coding: utf-8 -*-
''' Test active earth pressure coefficient values according to Mononobe-Okabe.
Results taken from the page:
https://eurocodeapplied.com/design/en1998/mononobe-okabe
.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import math
from geotechnics import mononobe_okabe

phi= math.radians(30) # design value of the angle of shearing resistance of soil.
delta_ad= 0.0 # design value of the friction angle between the soil and the structure.
psi= math.radians(90) #inclination angle of the wall back surface.
beta= 0 # inclination angle of the backfill top surface.
kh= 0.24 # horizontal seismic coefficient.
kv= -kh/2.0 # vertical seismic coefficient.
theta1, theta2= mononobe_okabe.theta_values(kh, kv)
theta= min(theta1, theta2)
ratio1= abs(theta-0.2110933332227465)/0.2110933332227465
# Mononobe-Okabe dynamic earth pressure coefficient for active state.
K_ad= mononobe_okabe.active_earth_pressure_coefficient_Kad(psi= psi, phi= phi, theta= theta, delta_ad= delta_ad, beta= beta)
ratio2= abs(K_ad-0.4856177257638489)/0.4856177257638489
# Mononobe-Okabe dynamic earth pressure coefficient for passive state.
K_pd= mononobe_okabe.passive_earth_pressure_coefficient_Kpd(psi= psi, phi= phi, theta= theta, beta= beta)
ratio3= abs(K_pd-2.6)/2.6

'''
print(theta, math.degrees(theta1), math.degrees(theta2))
print(ratio1)
print(K_ad)
print(ratio2)
print(K_pd)
print(ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and abs(ratio3)<1e-3:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
