# -*- coding: utf-8 -*-
''' Check get_horizontal_soil_reaction_diagram function.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from geotechnics import earth_pressure

# Define soil model.
phi= math.pi/6.0
rankineSoil= earth_pressure.RankineSoil(phi, rho= 2000)

# Define depth and tributary area corresponding to the spring simulating
# the soil reaction.
depth= 3.0 # 3 m
tributaryArea= 1.5 # 1.5 m2
Kh= 15e5 # horizontal subgrade modulus.

# Compute sample points.    
samplePoints, initStrain= earth_pressure.get_horizontal_soil_reaction_diagram(depth= depth, tributaryArea= tributaryArea, gamma= rankineSoil.gamma(), Ka= rankineSoil.Ka(), K0= rankineSoil.K0Jaky(), Kp= rankineSoil.Kp(), Kh= Kh)
refPoints= [(0.0, 29125.750500000006), (1.46119335, 29419.950000000004), (1.471, 44129.924999999996), (1.61809975, 264779.54999999993), (2.9419975, 267427.34549999994)]

err= 0.0
for pt, ref_pt in zip(samplePoints, refPoints):
    err+= (pt[0]-ref_pt[0])**2+(pt[1]-ref_pt[1])**2

E0= rankineSoil.K0Jaky()*tributaryArea*rankineSoil.gamma()*depth
err+= (samplePoints[2][0]-initStrain)**2+(samplePoints[2][1]-E0)**2

err= math.sqrt(err)

'''
print(samplePoints, err)
print(initStrain, E0)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')    

# #  FIGURES & REPORTS
# Ea= rankineSoil.Ka()*tributaryArea*rankineSoil.gamma()*depth
# Ep= rankineSoil.Kp()*tributaryArea*rankineSoil.gamma()*depth
# import matplotlib.pyplot as plt
# disp, forces= zip(*samplePoints)
# plt.title("Force-displacement diagram")
# plt.xlabel('D (m)')
# plt.ylabel('F (kN)')
# plt.plot(disp, [f * 1e-3 for f in forces])
# plt.plot(initStrain, E0*1e-3, 'go', label='at rest')
# plt.plot(0.1, Ea*1e-3, 'bo', label='active')
# plt.plot(2.5, Ep*1e-3, 'ro', label='passive')
# plt.grid()
# plt.legend()
# plt.show()

