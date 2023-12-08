# -*- coding: utf-8 -*-
''' Test for thermal loads according to Spanihsh IAPF standard.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.thermal import IAPF_thermal

annualTempVariation= IAPF_thermal.get_annual_temp_variation(deck_type= 'concrete_beams', z= 5, h= 1.8, s=2.5)

err= (annualTempVariation-43.6176116422547)/43.6176116422547
'''
print(annualTempVariation, err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
