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

GL28h= EC5_materials.GL28h

fm_k= GL28h.getCharacteristicBendingStrength()
ratio1= (fm_k-28e6)/28e6

kmod= GL28h.getKmod(loadDurationClass= 'long_term', serviceClass= 1)
ratio2= (kmod-0.7)/0.7

gammaM= GL28h.gammaM()
ratio3= (gammaM-1.25)/1.25

fm_d= GL28h.getDesignBendingStrength(loadDurationClass= 'long_term', serviceClass= 1)
ratio4= (fm_d-15680000)/15680000

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
