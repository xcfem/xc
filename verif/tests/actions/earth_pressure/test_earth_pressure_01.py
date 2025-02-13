# -*- coding: utf-8 -*-
'''Home-made test on earth pressures with various soil strata
'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

import math
from actions.earth_pressure import earth_pressure as ep

soil01= ep.EarthPressureModel(zGround=50, zBottomSoils=[40,30,20,10,-10], KSoils=[1.1,1.2,1.3,1.4,1.5], gammaSoils=[2,2.1,2.2,2.3,2.4], zWater=25, gammaWater=1)

zi= list(range(0,50,5))
zi.reverse() # Top down.

horizontal_pressure_01= dict()
for z in zi:
    horizontal_pressure_01[z]= soil01.getPressure(z)

vert_pressure_01= dict()
vert_pressure_01[45]= 2*5
vert_pressure_01[40]= 2*10
vert_pressure_01[35]= vert_pressure_01[40]+2.1*5
vert_pressure_01[30]= vert_pressure_01[40]+2.1*10
vert_pressure_01[25]= vert_pressure_01[30]+2.2*5
vert_pressure_01[20]= vert_pressure_01[25]+1.2*5
vert_pressure_01[15]= vert_pressure_01[20]+1.3*5
vert_pressure_01[10]= vert_pressure_01[20]+1.3*10
vert_pressure_01[5]= vert_pressure_01[10]+1.4*5
vert_pressure_01[0]= vert_pressure_01[10]+1.4*10

horizontal_pressure_01_tg= dict()
horizontal_pressure_01_tg[45]= 1.1*vert_pressure_01[45]
horizontal_pressure_01_tg[40]= 1.1*vert_pressure_01[40]
horizontal_pressure_01_tg[35]= 1.2*vert_pressure_01[35]
horizontal_pressure_01_tg[30]= 1.2*vert_pressure_01[30]
horizontal_pressure_01_tg[25]= 1.3*vert_pressure_01[25]
horizontal_pressure_01_tg[20]= 1.3*vert_pressure_01[20]+5
horizontal_pressure_01_tg[15]= 1.4*vert_pressure_01[15]+10
horizontal_pressure_01_tg[10]= 1.4*vert_pressure_01[10]+15
horizontal_pressure_01_tg[5]= 1.5*vert_pressure_01[5]+20
horizontal_pressure_01_tg[0]= 1.5*vert_pressure_01[0]+25

err01= 0.0
for z in zi:
    err01+= (horizontal_pressure_01[z]-horizontal_pressure_01_tg[z])**2
err01= math.sqrt(err01)

OK01= (err01<1e-6)

soil02=ep.EarthPressureModel(zGround=50, zBottomSoils=[40,30,20,10,-10],KSoils=[1.1,1.2,1.3,1.4,1.5],gammaSoils=[2,2.1,2.2,2.3,2.4], zWater=-1e3, gammaWater=1)

horizontal_pressure_02= dict()
for z in zi:
    horizontal_pressure_02[z]= soil02.getPressure(z)

vert_pressure_02= dict()
vert_pressure_02[45]= 2*5
vert_pressure_02[40]= 2*10
vert_pressure_02[35]= vert_pressure_02[40]+2.1*5
vert_pressure_02[30]= vert_pressure_02[40]+2.1*10
vert_pressure_02[25]= vert_pressure_02[30]+2.2*5
vert_pressure_02[20]= vert_pressure_02[25]+2.2*5
vert_pressure_02[15]= vert_pressure_02[20]+2.3*5
vert_pressure_02[10]= vert_pressure_02[20]+2.3*10
vert_pressure_02[5]= vert_pressure_02[10]+2.4*5
vert_pressure_02[0]= vert_pressure_02[10]+2.4*10

horizontal_pressure_02_tg= dict()
horizontal_pressure_02_tg[45]= 1.1*vert_pressure_02[45]
horizontal_pressure_02_tg[40]= 1.1*vert_pressure_02[40]
horizontal_pressure_02_tg[35]= 1.2*vert_pressure_02[35]
horizontal_pressure_02_tg[30]= 1.2*vert_pressure_02[30]
horizontal_pressure_02_tg[25]= 1.3*vert_pressure_02[25]
horizontal_pressure_02_tg[20]= 1.3*vert_pressure_02[20]
horizontal_pressure_02_tg[15]= 1.4*vert_pressure_02[15]
horizontal_pressure_02_tg[10]= 1.4*vert_pressure_02[10]
horizontal_pressure_02_tg[5]= 1.5*vert_pressure_02[5]
horizontal_pressure_02_tg[0]= 1.5*vert_pressure_02[0]

err02= 0.0
for z in zi:
    err02+= (horizontal_pressure_02[z]-horizontal_pressure_02_tg[z])**2
err02= math.sqrt(err02)

OK02= (err02<1e-6)

'''
print(err01, OK01)
print(err02, OK02)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (OK01 and OK02):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic output
# import matplotlib.pyplot as plt
# xi= list()
# y01i= list()
# y02i= list()
# for z in zi:
#     xi.append(z)
#     y01i.append(horizontal_pressure_01[z])
#     y02i.append(horizontal_pressure_02[z])
    
# plt.plot(y01i, xi, '-')
# plt.plot(y02i, xi, '.')

# plt.show()
