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

pres45=soil01.getPressure(45)
pres40=soil01.getPressure(40)
pres35=soil01.getPressure(35)
pres30=soil01.getPressure(30)
pres25=soil01.getPressure(25)
pres20=soil01.getPressure(20)
pres15=soil01.getPressure(15)
pres10=soil01.getPressure(10)
pres5=soil01.getPressure(5)
pres0=soil01.getPressure(0)


pres45_tg=1.1*2*5
pres40_tg=1.1*2*10
pres35_tg=pres40_tg+1.2*2.1*5
pres30_tg=pres40_tg+1.2*2.1*10
pres25_tg=pres30_tg+1.3*2.2*5
pres20_tg=pres25_tg+1.3*1.2*5+5
pres15_tg=pres20_tg+1.4*1.3*5+5
pres10_tg=pres20_tg+1.4*1.3*10+10
pres5_tg=pres10_tg+1.5*1.4*5+5
pres0_tg=pres10_tg+1.5*1.4*10+10

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if ((pres45-pres45_tg<1e-6) and (pres40-pres40_tg<1e-6)  and (pres35-pres35_tg<1e-6) and (pres30-pres30_tg<1e-6) and (pres25-pres25_tg<1e-6) and (pres20-pres20_tg<1e-6) and (pres15-pres15_tg<1e-6) and (pres10-pres10_tg<1e-6) and (pres5-pres5_tg<1e-6) and (pres0-pres0_tg<1e-6)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
