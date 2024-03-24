# -*- coding: utf-8 -*-
''' Test based on example 34-Q Characteristic thermal actions in bridges ‐ Consideration of thermal actions of the publication:

EN 1990, EN 1991 - Eurocodes 0-1 - Worked Examples. Carlo Sigmund. 2014. ISBN n.: 978-1-291-84215-9. https://eurocodespreadsheets.jimdo.com/tech-docs-en/
'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.thermal import EC1_thermal

## Uniform temperature component on bridges.
TmaxAir= 34 
TminAir= -18

bridgeTypes= [EC1_thermal.steel_deck_type, EC1_thermal.composite_deck_type,  EC1_thermal.concrete_deck_type]

temperatures= dict()
for bridgeType in bridgeTypes:
    TmaxBridge= EC1_thermal.getMinimumUniformBridgeTemperatureComponent(bridgeType= bridgeType, Tmin= TminAir)
    TminBridge= EC1_thermal.getMaximumUniformBridgeTemperatureComponent(bridgeType= bridgeType, Tmax= TmaxAir)
    temperatures[bridgeType]= (TminBridge, TmaxBridge)

refValues= {1: (50, -21), 2: (38.0, -14.0), 3: (36.0, -10.0)}

# Check results:
err= 0.0
for bridgeType in bridgeTypes:
    for i in [0,1]:
      t= temperatures[bridgeType][i]
      tRef= refValues[bridgeType][i]
      err+= (t-tRef)**2
err= math.sqrt(err)
      
# print(temperatures)
# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
