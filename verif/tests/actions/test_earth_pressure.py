# -*- coding: utf-8 -*-

import os
import xc_base
import geom
import xc
import math
from actions.earth_pressure import earth_pressure as ep


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

'''Home-made test on earth pressures with various soil strata
'''

soil01=ep.EarthPressureModel(zGround=50, zBottomSoils=[40,30,20,10,-10],KSoils=[1.1,1.2,1.3,1.4,1.5],gammaSoils=[2,2.1,2.2,2.3,2.4], zWater=25, gammaWater=1)

pres01_45=soil01.getPressure(45)
pres01_40=soil01.getPressure(40)
pres01_35=soil01.getPressure(35)
pres01_30=soil01.getPressure(30)
pres01_25=soil01.getPressure(25)
pres01_20=soil01.getPressure(20)
pres01_15=soil01.getPressure(15)
pres01_10=soil01.getPressure(10)
pres01_5=soil01.getPressure(5)
pres01_0=soil01.getPressure(0)

pres01_45_tg=1.1*2*5
pres01_40_tg=1.1*2*10
pres01_35_tg=pres01_40_tg+1.2*2.1*5
pres01_30_tg=pres01_40_tg+1.2*2.1*10
pres01_25_tg=pres01_30_tg+1.3*2.2*5
pres01_20_tg=pres01_25_tg+1.3*1.2*5+5
pres01_15_tg=pres01_20_tg+1.4*1.3*5+5
pres01_10_tg=pres01_20_tg+1.4*1.3*10+10
pres01_5_tg=pres01_10_tg+1.5*1.4*5+5
pres01_0_tg=pres01_10_tg+1.5*1.4*10+10

OK01= (pres01_45-pres01_45_tg<1e-6) and (pres01_40-pres01_40_tg<1e-6)  and (pres01_35-pres01_35_tg<1e-6) and (pres01_30-pres01_30_tg<1e-6) and (pres01_25-pres01_25_tg<1e-6) and (pres01_20-pres01_20_tg<1e-6) and (pres01_15-pres01_15_tg<1e-6) and (pres01_10-pres01_10_tg<1e-6) and (pres01_5-pres01_5_tg<1e-6) and (pres01_0-pres01_0_tg<1e-6)

soil02=ep.EarthPressureModel(zGround=50, zBottomSoils=[40,30,20,10,-10],KSoils=[1.1,1.2,1.3,1.4,1.5],gammaSoils=[2,2.1,2.2,2.3,2.4], zWater=-1e3, gammaWater=1)

pres02_45=soil02.getPressure(45)
pres02_40=soil02.getPressure(40)
pres02_35=soil02.getPressure(35)
pres02_30=soil02.getPressure(30)
pres02_25=soil02.getPressure(25)
pres02_20=soil02.getPressure(20)
pres02_15=soil02.getPressure(15)
pres02_10=soil02.getPressure(10)
pres02_5=soil02.getPressure(5)
pres02_0=soil02.getPressure(0)

pres02_45_tg= 1.1*2*5
pres02_40_tg= 1.1*2*10
pres02_35_tg=pres02_40_tg+1.2*2.1*5
pres02_30_tg=pres02_40_tg+1.2*2.1*10
pres02_25_tg=pres02_30_tg+1.3*2.2*5
pres02_20_tg=pres02_25_tg+1.3*2.2*5
pres02_15_tg=pres02_20_tg+1.4*2.3*5
pres02_10_tg=pres02_20_tg+1.4*2.3*10
pres02_5_tg=pres02_10_tg+1.5*2.4*5
pres02_0_tg=pres02_10_tg+1.5*2.4*10

OK02= (pres02_45-pres02_45_tg<1e-6) and (pres02_40-pres02_40_tg<1e-6)  and (pres02_35-pres02_35_tg<1e-6) and (pres02_30-pres02_30_tg<1e-6) and (pres02_25-pres02_25_tg<1e-6) and (pres02_20-pres02_20_tg<1e-6) and (pres02_15-pres02_15_tg<1e-6) and (pres02_10-pres02_10_tg<1e-6) and (pres02_5-pres02_5_tg<1e-6) and (pres02_0-pres02_0_tg<1e-6)

'''
print(OK02)
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (OK01 and OK02):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
