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

C18= EC5_materials.C18

fc_90_k= C18.getCharacteristicCompressiveStrengthPerpendicularToTheGrain()
ratio1= (fc_90_k-2.2e6)/2.2e6

kmod= C18.getKmod(loadDurationClass= 'permanent', serviceClass= 3)
ratio2= (kmod-0.5)/0.5

gammaM= C18.gammaM()
ratio3= (gammaM-1.3)/1.3

fc_90_d= C18.getDesignCompressiveStrengthPerpendicularToTheGrain(loadDurationClass= 'permanent', serviceClass= 3)
ratio4= (fc_90_d-846153.8461538461)/846153.8461538461

'''
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
print('fc_90_d= ', fc_90_d)
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12 and abs(ratio4)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
