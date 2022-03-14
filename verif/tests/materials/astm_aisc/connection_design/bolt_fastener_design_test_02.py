# -*- coding: utf-8 -*-
# COMPANION TO THE AISC STEEL CONSTRUCTION MANUAL
# Volume 1: Design Examples
# EXAMPLE J.3 COMBINED TENSION AND SHEAR IN BEARING TYPE CONNECTIONS

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.astm_aisc import ASTM_materials
from misc_utils import units_utils

#Materials
## Steel material
steel= ASTM_materials.A325
steel.gammaM= 1.00
diameter= 3/4.0*units_utils.inchToMeter
bolt=  ASTM_materials.BoltFastener(diameter, steelType= steel)
Tu= 23.4*units_utils.kipToN
Vu= 8.0*units_utils.kipToN

Rn= bolt.getNominalCombinedStrength(V= Vu, threadsExcluded=False)
RnRef= 33.9*units_utils.kipToN
ratio1= abs(Rn-RnRef)/RnRef

'''
print('Rn= ', Rn/units_utils.kipToN)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<5e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
