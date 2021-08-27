# -*- coding: utf-8 -*-
''' Elementary test for wood materials according to Eurocode 5.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato"
__copyright__= "Copyright 2021, LCPT AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from materials.ec5 import EC5_materials

C24= EC5_materials.C24

fm_k= C24.getCharacteristicBendingStrength()
ratio1= (fm_k-24e6)/24e6

kmod= C24.getKmod(loadDurationClass= 'medium_term', serviceClass= 1)
ratio2= (kmod-0.8)/0.8

gammaM= C24.gammaM()
ratio3= (gammaM-1.3)/1.3

fm_d= C24.getDesignBendingStrength(loadDurationClass= 'medium_term', serviceClass= 1)
ratio4= (fm_d-14769230.769230768)/14769230.769230768

'''
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
print('fm_d= ', fm_d)
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12 and abs(ratio4)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
