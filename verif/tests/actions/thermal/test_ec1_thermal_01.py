# -*- coding: utf-8 -*-
''' Test for thermal loads according to Eurocode 1: Actions on structures - Part 1-5: General actions - Thermal actions.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.thermal import EC1_thermal

## Uniform temperature component on bridges.
climateZn= 4 # Climate zone for Madrid according to figure AN.2 of Spanish National Annex.
height= 590 # Height above sea level.
TmaxAir= 44 # Minimum shade air temperature in MADRID according to figure AN.1 of Spanish National Annex.
TminAir= EC1_thermal.getMinAnnualShadeAirTemp(climateZn= climateZn, height= height)
TmaxBridge= EC1_thermal.getMaxAnnualUniformBridgeTemp(tMax= TmaxAir, returnPeriod= 100)
TminBridge= EC1_thermal.getMinAnnualUniformBridgeTemp(climateZn= climateZn, height= height, returnPeriod= 100)
T0= 15 # initial bridge temperature
ATcon= T0-TminBridge # characteristic value of the maximum contraction range.
ATexp= TmaxBridge-T0 # characteristic value of the maximum expansion range.

## Temperature difference components on bridges.
deltaTMheat= EC1_thermal.getLinearTemperatureDifferenceComponent(bridgeType=EC1_thermal.concrete_beam, topWarmerThanBottom= True, surfacingDepth= 20e-3, ballast= False, waterproofedOnly= False) # ballast= False because is not extended over the pergola surface.
deltaTMcool= EC1_thermal.getLinearTemperatureDifferenceComponent(bridgeType=EC1_thermal.concrete_beam, topWarmerThanBottom= False, surfacingDepth= 20e-3, ballast= False, waterproofedOnly= False) # ballast= False because is not extended over the pergola surface.

# Check computed values.
err= (TminAir+11.9)**2
err+= (TmaxBridge-45.6987676947775)**2
err+= (TminBridge+13.21641702458804)**2
err+= (ATcon-28.21641702458804)**2
err+= (ATexp-30.6987676947775)**2
err+= (deltaTMheat-13.2)**2
err+= (deltaTMcool-8.48)**2
err= math.sqrt(err)

'''
print('  maximum shade air temperature (T= 50 years): ', TmaxAir)
print('  minimum shade air temperature (T= 50 years): ', TminAir)
print('  maximum bridge temperature (T= 100 years): ', TmaxBridge)
print('  minimum bridge temperature (T= 100 years): ', TminBridge)
print('  annual temperature variation (T= 100 years): ', (TmaxBridge-TminBridge))
print('  characteristic value of the maximum contraction range: ', ATcon)
print('  characteristic value of the maximum expansion range: ', ATexp)
print('  top warmer than bottom temperature increment: ', deltaTMheat)
print('  bottom warmer than top temperature increment: ', deltaTMcool)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
