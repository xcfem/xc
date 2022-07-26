# -*- coding: utf-8 -*-
''' Sheet-piles according to chapter 14 of the book "Principles of Foundation 
Engineering" of Braja M. Das. Eight Edition. CENGAGE Learning. 2016.

Example 14.7
'''

from __future__ import division
from __future__ import print_function

import math
from geotechnics import earth_pressure
from geotechnics.earth_retaining import sheet_pile
from scipy.constants import g

# Trick to simulate the absence of water table
rho= 16.0e3/g
waterDensity= 1e3 # kg/m3
fakeSatRho= rho+waterDensity

soil= earth_pressure.RankineSoil(phi= math.radians(30), rho= rho, rhoSat= fakeSatRho)
L1= 3.05 # m
L2= 6.10 # m

Ka= soil.Ka()
ratio1= abs(Ka-1.0/3.0)/(1/3.0)
Kp= soil.Kp()
ratio2= abs(Kp-3)/3

# Sheet-pile object.
sheetPile= sheet_pile.AnchoredSheetPileWall(soil= soil, waterTableDepth= L1, excavationDepth= L1+L2, anchorDepth= 1.52)
    
# unit weight
gamma= soil.gamma()
gammaSum= soil.submergedGamma()

# intensity of the active pressure at a depth L1
sigma_p_1= sheetPile.getActivePressureAtDepth(z= L1)
ratio3= abs(sigma_p_1-16.266666666666673e3)/16.266666666666673e3
# intensity of the active pressure at a depth L1+L2
sigma_p_2= sheetPile.getActivePressureAtDepth(z= sheetPile.excavationDepth)
ratio4= abs(sigma_p_2-48.80000000000001e3)/48.80000000000001e3

# Depth of zero net pressure.
L3= sheetPile.getZeroNetPressureDepth()
ratio5= abs(L3-1.1437500000000005)/1.1437500000000005

## Active side pressure.
P, z_bar= sheetPile.getActiveSidePressure()
ratio6= abs(P-251.16750000000005e3)/251.16750000000005e3
ratio7= abs(z_bar-3.8125)/3.8125

# Compute L4
L4= sheetPile.getL4()
ratioL4= abs(L4-2.374825083149082)/2.374825083149082
# Theoretical depth
Dtheory= sheetPile.getDepth(depthSafetyFactor= 1.0)
# Actual depth
Dactual= sheetPile.getDepth(depthSafetyFactor= 1.3)
# Total length
L= sheetPile.getTotalLength(depthSafetyFactor= 1.3)
# Anchor force per unit length of the wall
F= sheetPile.getAnchorForcePerUnitLength()
ratioF= abs(F-130.85189092151379e3)/130.85189092151379e3
# Zero shear depth
z= sheetPile.getZeroShearDepth()
ratioZ= abs(z-7.00495960699044)/7.00495960699044
# Maximum bending moment.
Mmax= sheetPile.getMaxBendingMoment()
ratioMmax= abs(Mmax-412.17993273498126e3)/412.17993273498126e3

'''
print('Ka= ', Ka)
print('ratio1= ', ratio1)
print('Kp= ', Kp)
print('ratio2= ', ratio2)
print('gamma= ', gamma/1e3, 'kN/m3')
print('gammaSum= ', gammaSum/1e3, 'kN/m3')
print('sigma_p_1= ', sigma_p_1/1e3, 'kPa')
print('ratio3= ', ratio3)
print('sigma_p_2= ', sigma_p_2/1e3, 'kPa')
print('ratio4= ', ratio4)
print('L3= ', L3, 'm')
print('ratio5= ', ratio5)
print('P= ', P/1e3, 'kN')
print('ratio6= ', ratio6)
print('z_bar= ', z_bar, 'm')
print('ratio7= ', ratio7)
print('L4= ', L4, 'm')
print('ratioL4= ', ratioL4)
print('Dtheory= ', Dtheory, 'm')
print('Dactual= ', Dactual, 'm')
print('L= ', L, 'm')
print('F= ', F/1e3, 'kN')
print('ratioF= ', ratioF)
print('z= ', z, 'm')
print('ratioZ= ', ratioZ)
print('Mmax= ', Mmax/1e3,'kN.m/m')
print('ratioMmax= ', ratioMmax)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5)<1e-10 and abs(ratio6)<1e-10 and abs(ratio7)<1e-10 and abs(ratioL4)<1e-10 and abs(ratioF)<1e-10 and abs(ratioZ)<1e-10 and abs(ratioMmax)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
