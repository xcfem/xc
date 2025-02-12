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

pres01= dict()
for z in zi:
    pres01[z]= soil01.getPressure(z)

pres01_tg= dict()
pres01_tg[45]=1.1*2*5
pres01_tg[40]=1.1*2*10
pres01_tg[35]=pres01_tg[40]+1.2*2.1*5
pres01_tg[30]=pres01_tg[40]+1.2*2.1*10
pres01_tg[25]=pres01_tg[30]+1.3*2.2*5
pres01_tg[20]=pres01_tg[25]+1.3*1.2*5+5
pres01_tg[15]=pres01_tg[20]+1.4*1.3*5+5
pres01_tg[10]=pres01_tg[20]+1.4*1.3*10+10
pres01_tg[5]=pres01_tg[10]+1.5*1.4*5+5
pres01_tg[0]=pres01_tg[10]+1.5*1.4*10+10

err01= 0.0
for z in zi:
    err01+= (pres01[z]-pres01_tg[z])**2
err01= math.sqrt(err01)

OK01= (err01<1e-6)

soil02=ep.EarthPressureModel(zGround=50, zBottomSoils=[40,30,20,10,-10],KSoils=[1.1,1.2,1.3,1.4,1.5],gammaSoils=[2,2.1,2.2,2.3,2.4], zWater=-1e3, gammaWater=1)

pres02= dict()
for z in zi:
    pres02[z]= soil02.getPressure(z)

pres02_tg= dict()
pres02_tg[45]= 1.1*2*5
pres02_tg[40]= 1.1*2*10
pres02_tg[35]= pres02_tg[40]+1.2*2.1*5
pres02_tg[30]= pres02_tg[40]+1.2*2.1*10
pres02_tg[25]= pres02_tg[30]+1.3*2.2*5
pres02_tg[20]= pres02_tg[25]+1.3*2.2*5
pres02_tg[15]= pres02_tg[20]+1.4*2.3*5
pres02_tg[10]= pres02_tg[20]+1.4*2.3*10
pres02_tg[5]= pres02_tg[10]+1.5*2.4*5
pres02_tg[0]= pres02_tg[10]+1.5*2.4*10

err02= 0.0
for z in zi:
    err02+= (pres02[z]-pres02_tg[z])**2
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
#     y01i.append(pres01[z])
#     y02i.append(pres02[z])
    
# plt.plot(y01i, xi, '-')
# plt.plot(y02i, xi, '.')

# plt.show()
