# -*- coding: utf-8 -*-
'''Based on the problem "Lateral-09: Strata with different parameters." of the 
   publication: "300 Solved Problems Soil / Rock Mechanics and Foundations
   Engineering" Copyright by L. Prieto-Portar, October, 2009.

https://bouassidageotechnics.wordpress.com/wp-content/uploads/2016/04/300_solved_problems_in_geotechnical_engineering.pdf
'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

import math
from scipy.constants import g
from actions.earth_pressure import earth_pressure as ep

ft= .3048 # m
pcf= 0.157087464e3 # N/m^3 
ksf= 47.880258888889e3 # Pa

H= 20*ft
Ka1= 0.333
Ka2= 0.217
gammaSoil1= 115*pcf
gammaSoil2= 125*pcf
gammaWater= 1e3*g
q= 2.5*ksf

soil= ep.EarthPressureModel(zGround= H, zBottomSoils=[H/2.0,0], KSoils=[Ka1, Ka2], gammaSoils=[gammaSoil1, gammaSoil2], zWater= H, gammaWater= gammaWater, qUnif= q)


zA= H # a point.
zB= H/2.0 # b point.
zBUp= zB+1e-3
zBDown= zB-1e-3
zC= 0.0 # c point.

pa= soil.getPressure(zA)
pbUp= soil.getPressure(zBUp)
pb_water= soil.getWaterPressure(zB)
pbUp_effective= soil.getEffectivePressure(zBUp)
pbDown= soil.getPressure(zBDown)
pbDown_effective= soil.getEffectivePressure(zBDown)
pc= soil.getPressure(zC)
pc_effective= soil.getEffectivePressure(zC)

err= math.sqrt((pa-39.86031552500009e3)**2+(pbUp_effective-48.24298344033246e3)**2+(pbDown_effective-31.441544868521046e3)**2+(pc_effective-37.94065695883805e3)**2)

'''
print('q= ', q/1e3, 'kN/m2')
print('gammaSoil1= ', gammaSoil1/1e3,'kN/m3')
print('gammaSoil2= ', gammaSoil2/1e3,'kN/m3')
print('\npa= ', pa/1e3,' kPa (', pa/ksf,' ksf)')
print('water pressure at point b: ', pb_water/1e3,' kPa (', pb_water/ksf,' ksf)')
print("pb+= ", pbUp/1e3,' kPa (', pbUp/ksf,' ksf)')
print("pb'+= ", pbUp_effective/1e3,' kPa (', pbUp_effective/ksf,' ksf)')
print("pb-= ", pbDown/1e3,' kPa (', pbDown/ksf,' ksf)')
print("pb'-= ", pbDown_effective/1e3,' kPa (', pbDown_effective/ksf,' ksf)')
print("pc= ", pc/1e3,' kPa (', pc/ksf,' ksf)')
print("pc'= ", pc_effective/1e3,' kPa (', pc_effective/ksf,' ksf)')
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic output
# import matplotlib.pyplot as plt
# xi= list()
# y01i= list()
# y02i= list()
# zi= [H, zBUp, zBDown, zC]
# for z in zi:
#     xi.append(z)
#     y01i.append(soil.getEffectivePressure(z)/1e3)
#     y02i.append(soil.getPressure(z)/1e3)
    
# plt.plot(y01i, xi, '-')
# plt.plot(y02i, xi, '.')

# plt.show()
