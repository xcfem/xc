# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import math
from geotechnics import mononobe_okabe


H= 7.6 # height of the structure.
kh= 0.096 # seismic coefficient of horizontal acceleration.
kv= kh/2.0 # seismic coefficient of vertical acceleration.
Kas= 1.0/3.0 #static earth pressure coefficient
gamma_soil= 20e3 #unit weight of soil (backfill)
psi= math.radians(90) #back face inclination of the structure (<= PI/2)
beta= 0 #slope inclination of backfill.
delta_ad= 0 #angle of friction soil - structure.
theta1= math.radians(5.3)
theta2= math.radians(5.8)
phi= math.radians(29.0985) #angle of internal friction of soil.
phi_d= math.atan(math.tan(phi)/1.25)
L= 12.5

K_ad1= mononobe_okabe.active_earth_pressure_coefficient_Kad(psi, phi_d, theta1, delta_ad,beta)
K_ad2= mononobe_okabe.active_earth_pressure_coefficient_Kad(psi, phi_d, theta2, delta_ad,beta)
AEd1= mononobe_okabe.overpressure_dry(H, gamma_soil, kv, kh, psi, phi_d, delta_ad,beta,Kas)
AEd2= mononobe_okabe.overpressure_dry(H, gamma_soil, -kv, kh, psi, phi_d, delta_ad,beta,Kas)


K_ad1Teor= 0.487898697512
ratio1= abs(K_ad1-K_ad1Teor)/K_ad1Teor
K_ad2Teor= 0.494882910177
ratio2= abs(K_ad2-K_ad2Teor)/K_ad2Teor
AEd1Teor= 1278.02671863e3/L
ratio3= abs(AEd1-AEd1Teor)/AEd1Teor

'''
print("theta1= ", math.degrees(theta1))
print("phi_d= ", math.degrees(phi_d))
print("K_ad1=", K_ad1)
print("K_ad2=", K_ad2)
print("AEd1=", AEd1/1e3*L, " kN")
print("AEd2=", AEd2/1e3*L, " kN")
print("ratio1= ", ratio1)
print("ratio2= ", ratio2)
print("ratio3= ", ratio3)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-3 and abs(ratio2)<1e-3 and abs(ratio3)<1e-3:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
