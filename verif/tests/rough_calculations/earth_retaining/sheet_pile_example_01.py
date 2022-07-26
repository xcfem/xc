# -*- coding: utf-8 -*-
''' Sheet-piles according to chapter 14 of the book "Principles of Foundation 
Engineering" of Braja M. Das. Eight Edition. CENGAGE Learning. 2016.

Example 14.1
'''

from __future__ import division
from __future__ import print_function

import math
from geotechnics import earth_pressure
from geotechnics.earth_retaining import sheet_pile
from scipy.constants import g

soil= earth_pressure.RankineSoil(phi= math.radians(32), rho= 15.90e3/g, rhoSat= 19.33e3/g)
L1= 2.0 # m
L2= 3.0 # m

Ka= soil.Ka()
ratio1= abs(Ka-0.3072585245224683)/0.3072585245224683
Kp= soil.Kp()
ratio2= abs(Kp-3.2545883032998644)/3.2545883032998644

# Sheet-pile object.
sheetPile= sheet_pile.CantileverSheetPileWall(title= 'Example 14.1', soil= soil, waterTableDepth= L1, excavationDepth= L1+L2)
    
# unit weight
gamma= soil.gamma()
gammaSum= soil.submergedGamma()

# intensity of the active pressure at a depth L1
sigma_p_1= sheetPile.getActivePressureAtDepth(z= L1)
ratio3= abs(sigma_p_1-9.770821079814493e3)/9.770821079814493e3
# intensity of the active pressure at a depth L1+L2
sigma_p_2= sheetPile.getActivePressureAtDepth(z= sheetPile.excavationDepth)
ratio4= abs(sigma_p_2-18.54921248834764e3)/18.54921248834764e3

# Depth of zero net pressure.
L3= sheetPile.getZeroNetPressureDepth()
ratio5= abs(L3-0.6608562451286827)/0.6608562451286827

## Active side pressure.
P, z_bar= sheetPile.getActiveSidePressure()
ratio6= abs(P-58.380052889629425e3)/58.380052889629425e3
ratio7= abs(z_bar-2.230098685234527)/2.230098685234527

# Left side pressure diagram.
A0, A1, A2, A3, A4= sheetPile.getL4PolynomialCoeffs()
ratioA1= abs(A1-7.66087360478114)/7.66087360478114
ratioA2= abs(A2+16.639336065524226)/16.639336065524226
ratioA3= abs(A3+151.26493007344456)/151.26493007344456
ratioA4= abs(A4+230.51032363941252)/230.51032363941252
# Compute L4
L4= sheetPile.getL4()
ratioL4= abs(L4-4.74111008501169)/4.74111008501169
# Theoretical depth
D= sheetPile.getDepth(depthSafetyFactor= 1.0)
# Total length
L= sheetPile.getTotalLength(depthSafetyFactor= 1.3)

# Maximum bending moment.
Mmax= sheetPile.getMaxBendingMoment()
ratioMmax= abs(Mmax-209.57330322152927e3)/209.57330322152927e3

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
print('A1= ', A1)
print('ratioA1= ', ratioA1)
print('A2= ', A2)
print('ratioA2= ', ratioA2)
print('A3= ', A3)
print('ratioA3= ', ratioA3)
print('A4= ', A4)
print('ratioA4= ', ratioA4)
print('L4= ', L4, 'm')
print('ratioL4= ', ratioL4)
print('D= ', D, 'm')
print('L= ', L, 'm')
print('Mmax= ', Mmax/1e3,'kN.m/m')
print('ratioMmax= ', ratioMmax)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10 and abs(ratio4)<1e-10 and abs(ratio5)<1e-10 and abs(ratio6)<1e-10 and abs(ratio7)<1e-10 and abs(ratioA1)<1e-10 and abs(ratioA2)<1e-10 and abs(ratioA3)<1e-10 and abs(ratioA4)<1e-10 and abs(ratioL4)<1e-10 and abs(ratioMmax)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
