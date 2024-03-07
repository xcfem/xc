# -*- coding: utf-8 -*-
''' Sheet-piles according to chapter 14 of the book "Principles of Foundation 
Engineering" of Braja M. Das. Eight Edition. CENGAGE Learning. 2016. ISBN-13: 978-1-305-08155-0

Example 14.2 page 723 (same as example 14.1 assuming the absence of water table).
'''

from __future__ import division
from __future__ import print_function

import math
from geotechnics import earth_pressure
from geotechnics.earth_retaining import sheet_pile
from scipy.constants import g

soil= earth_pressure.RankineSoil(phi= math.radians(32), rho= 15.90e3/g, rhoSat= 19.33e3/g)
L= 5.0 # m

Ka= soil.Ka()
ratio1= abs(Ka-0.3072585245224683)/0.3072585245224683
Kp= soil.Kp()
ratio2= abs(Kp-3.2545883032998644)/3.2545883032998644

# Sheet-pile object.
sheetPile= sheet_pile.CantileverSheetPileWall(title= 'Example 14.2', soil= soil, waterTableDepth= 2*L, excavationDepth= L)
    
# unit weight
gamma= soil.gamma()

# intensity of the active pressure at a depth L
sigma_p_2= sheetPile.getActivePressureAtDepth(z= sheetPile.excavationDepth)
ratio4= abs(sigma_p_2-24.42705269953623e3)/24.42705269953623e3

# Depth of zero net pressure.
L3= sheetPile.getZeroNetPressureDepth()
ratioL3= abs(L3-0.5212489737913287)/0.5212489737913287

## Active side pressure.
P, z_bar= sheetPile.getActiveSidePressure()
ratio6= abs(P-67.43391982503054e3)/67.43391982503054e3
ratio7= abs(z_bar-2.014165982527552)/2.014165982527552

# Left side pressure diagram.
A0, A1, A2, A3, A4= sheetPile.getL4PolynomialCoeffs()
ratioA1= abs(A1-6.042497947582659)/6.042497947582659
ratioA2= abs(A2+11.511781446540626)/11.511781446540626
ratioA3= abs(A3+86.9498947046773)/86.9498947046773
ratioA4= abs(A4+113.36148156366544)/113.36148156366544
# Compute L4
L4= sheetPile.getL4()
ratioL4= abs(L4-4.007143611343835)/4.007143611343835
# Theoretical depth
D= sheetPile.getDepth(depthSafetyFactor= 1.0)
# Total length
L= sheetPile.getTotalLength(depthSafetyFactor= 1.3)

# Maximum bending moment.
Mmax= sheetPile.getMaxBendingMoment()
ratioMmax= abs(Mmax-212.0886594633984e3)/212.0886594633984e3

'''
print('Ka= ', Ka)
print('ratio1= ', ratio1)
print('Kp= ', Kp)
print('ratio2= ', ratio2)
print('gamma= ', gamma/1e3, 'kN/m3')
print('sigma_p_2= ', sigma_p_2/1e3, 'kPa')
print('ratio4= ', ratio4)
print('L3= ', L3, 'm')
print('ratioL3= ', ratioL3)
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
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio4)<1e-10 and abs(ratioL3)<1e-10 and abs(ratio6)<1e-10 and abs(ratio7)<1e-10 and abs(ratioA1)<1e-10 and abs(ratioA2)<1e-10 and abs(ratioA3)<1e-10 and abs(ratioA4)<1e-10 and abs(ratioL4)<1e-10 and abs(ratioMmax)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
