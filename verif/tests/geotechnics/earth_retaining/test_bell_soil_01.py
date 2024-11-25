# -*- coding: utf-8 -*-
''' Check BellSoil class.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc
from geotechnics import earth_pressure
from scipy.constants import g

# Define soil model.
gammaMc= 1.25
testSoilA= earth_pressure.BellSoil(phi= math.radians(24), c= 30e3/gammaMc, rho= 19.5e3/g, rhoSat= 19.5e3/g, Kh= 120e6)
testSoilB= earth_pressure.BellSoil(phi= math.radians(24), c= 30e3, rho= 19.5e3/g, rhoSat= 19.5e3/g, Kh= 120e6, gammaMc= gammaMc)

Ea_a, E0_a, Ep_a= testSoilA.getEarthThrusts(sg_v= 1e6, tributaryArea= 1.0, designValue= False)
Ea_b, E0_b, Ep_b= testSoilB.getEarthThrusts(sg_v= 1e6, tributaryArea= 1.0, designValue= True)

err= math.sqrt((Ea_a-Ea_b)**2+(E0_a-E0_b)**2+(Ep_a-Ep_b)**2)

'''
print('Ea= ', Ea_a/1e3)
print('E0= ', E0_a/1e3)
print('Ep= ', Ep_a/1e3)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

