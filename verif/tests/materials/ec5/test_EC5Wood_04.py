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
sClass= 1 # service class.

fv_k= C24.getCharacteristicShearStrength()
ratio1= (fv_k-2.5e6)/2.5e6

kmod= C24.getKmod(loadDurationClass= 'medium_term', serviceClass= sClass)
ratio2= (kmod-0.8)/0.8

gammaM= C24.gammaM()
ratio3= (gammaM-1.3)/1.3

fv_d= C24.getDesignShearStrength(loadDurationClass= 'medium_term', serviceClass= sClass)
ratio4= (fv_d-1538461.5384615385)/1538461.5384615385

'''
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
print('fv_d= ', fv_d)
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12 and abs(ratio4)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
