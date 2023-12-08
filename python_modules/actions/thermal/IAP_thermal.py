# -*- coding: utf-8 -*-
'''Functions to compute thermal loads according to Spanish IAP-11 code.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO) "
__copyright__= "Copyright 2019,  LCPT A_OO "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import math
import scipy.interpolate
from actions.thermal import EC1_thermal

# Table 4.3-a: Minimal annual temperature (Celsius degrees)
climateZone= EC1_thermal.climateZone # climate zone according to figure 4.3-b
height= EC1_thermal.height

minAnnualTemp= EC1_thermal.minAnnualShadeAirTemp


fMinAnnualTemp= EC1_thermal.fMinAnnualShadeAirTemp

# Uniform temperature component on bridges.
getMinAnnualTemp= EC1_thermal.getMinAnnualUniformBridgeTemp
getMaxAnnualTemp= EC1_thermal.getMaxAnnualUniformBridgeTemp
