# -*- coding: utf-8 -*-
''' Cohesive soil example inspired in the problem called "Numerical 3" from the
    page 24 of the document:

    https://abhashacharya.com.np/wp-content/uploads/2021/05/Sheet-Piles-with-Numericals.pdf
'''

import math
from geotechnics import frictional_cohesive_soil as fcs
from scipy.constants import g

# Problem geometry. 
H= 6.0
D= 2.88

# Soil
qu= 70e3 # unconfined compressive strength.
Cu= qu/2.0
soil= fcs.FrictionalCohesiveSoil(phi= 0.0, c= Cu, rho= 17e3/g)

alphaAngle= 0.0 #angle of the back of the retaining wall (radians).
backFillSlope= 0.0 # slope of the backfill (radians).
wallFrictionAngle= 0.0 # friction angle between soil an back of retaining wall (radians).

h0= soil.getCoulombTensionCrackDepth(sg_v= 0.0, a= alphaAngle, b= backFillSlope, d= wallFrictionAngle)
h0Ref= 4.12
ratio0= abs(h0-h0Ref)/h0Ref
Ka= soil.Ka_coulomb(a= alphaAngle, b= backFillSlope, d= wallFrictionAngle)
KaRef= 1.0
ratio1= abs(Ka-KaRef)/KaRef
## Horizontal active pressure.
sg_v= soil.gamma()*H
eah= soil.eah_coulomb(sg_v= sg_v, a= alphaAngle, b= backFillSlope, d= wallFrictionAngle)
Pa= 1/2.0*eah*(H-h0)
PaRef= 30.08e3
ratio2= abs(Pa-PaRef)/PaRef

Kp= soil.Kp_coulomb(a= alphaAngle, b= backFillSlope, d= wallFrictionAngle)
KpRef= 1.0
ratio3= abs(Kp-KpRef)/KpRef
## Horizontal passive pressure

### At the top (sg_v= 0)
sg_v= 0.0
eph0= soil.eph_coulomb(sg_v= sg_v, a= alphaAngle, b= backFillSlope, d= wallFrictionAngle)
eph0Ref= 2*Cu
ratio4= abs(eph0-eph0Ref)/eph0Ref
### At the bottom (sg_v= soil.gamma()*D)
sg_v= soil.gamma()*D
ephD= soil.eph_coulomb(sg_v= sg_v, a= alphaAngle, b= backFillSlope, d= wallFrictionAngle)
ephDRef= 2*Cu+Kp*sg_v
ratio5= abs(ephD-ephDRef)/ephDRef

'''
print('h0= ', h0)
print('ratio0= ', ratio0)
print('Ka= ', Ka)
print('ratio1= ', ratio1)
print('Pa= ', Pa/1e3, 'kN')
print('ratio2= ', ratio2)
print('Kp= ', Kp)
print('ratio3= ', ratio3)
print('eph0= ', eph0/1e3, 'kN')
print('eph0Ref= ', eph0Ref/1e3, 'kN')
print('ratio4= ', ratio4)
print('ephD= ', ephD/1e3, 'kN')
print('ephDRef= ', ephDRef/1e3, 'kN')
print('ratio5= ', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-3) and (abs(ratio1)<1e-12) and (abs(ratio2)<1e-2) and (abs(ratio3)<1e-12) and (abs(ratio4)<1e-12) and (abs(ratio5)<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
