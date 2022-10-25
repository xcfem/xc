# -*- coding: utf-8 -*-
'''
   Verification of the active pressure formula for cohesive soils.

   Based on the example 13.18 from https://faculty.ksu.edu.sa/sites/default/files/ce_481_lateral_earth_pressure_0.pdf
'''

import math
from geotechnics import frictional_cohesive_soil as fcs
from scipy.constants import g


wallHeight= 4.0 #m
soilGamma= 15e3 # N/m3
soilRho= soilGamma/g
fi= math.radians(26)
cohesion= 8e3 # N/m2

q= 10e3 # surface load

soil= fcs.FrictionalCohesiveSoil(phi= fi, c= cohesion, rho= soilRho)

crackDepth= soil.getCoulombTensionCrackDepth(sg_v=q, a= 0, b= 0)

ka= soil.Ka_coulomb(a= 0,b= 0,d= 0)
sigma_a= soil.eah_coulomb(sg_v= q, a= 0, b= 0)
sigma_0= soil.eah_coulomb(sg_v=crackDepth*soil.gamma()+q, a= 0, b= 0)
sigma_b= soil.eah_coulomb(sg_v=wallHeight*soil.gamma()+q, a= 0, b= 0)
earthPressureForce= sigma_b*(wallHeight-crackDepth)/2.0

# Reference values.
sigma_A= ka*q-2*soil.getDesignC()*math.sqrt(ka)
sigma_B= ka*(q+soil.gamma()*wallHeight)-2*soil.getDesignC()*math.sqrt(ka)
z0= wallHeight/(sigma_B/-sigma_A+1)
P= sigma_B*(wallHeight-z0)/2.0

ratio1= abs(sigma_B-sigma_b)/sigma_B
ratio2= abs(z0-crackDepth)/z0
ratio3= abs(earthPressureForce-P)/P

'''
print(sigma_A)
print(sigma_B)
print(ratio1)
print('crack depth ka= ',ka) 
print('crack depth z0= ',crackDepth, ' m') 
print(z0)
print(ratio2)
print('horizontal pressure at the top of the wall sigma_a= ',sigma_a/1e3, ' kN/m2') 
print('horizontal pressure at crack depth sigma_0= ',sigma_0/1e3, ' kN/m2') 
print('horizontal pressure at the bottom of the wall sigma_b= ',sigma_b/1e3, ' kN/m2') 
print('earth pressure force P= ',earthPressureForce/1e3, ' kN/m')
print(P)
print(ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and abs(ratio2<1e-12) and abs(ratio3<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
