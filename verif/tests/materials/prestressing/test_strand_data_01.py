# -*- coding: utf-8 -*-
''' Prestressing strand mechanical properties test.'''
from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from materials.ehe import EHE_materials

strand= EHE_materials.Y1860S7Strand_15_3
ECordon= 205000e6
Fm= strand.Fm()
ratio1= abs(Fm-260.4e3)/260.4e3
massPerMeter= strand.massPerMeter()
ratio2= abs(massPerMeter-1.093)/1.093
Fp= strand.Fp()
ratio3= abs(Fp-224e3)/224e3

'''
print(ECordon, strand.Es)
print('Fm= ', Fm/1e3, ' kN')
print('ratio1= ', ratio1)
print('mass per meter: ', massPerMeter)
print('ratio2= ', ratio2)
print('Fp= ', Fp/1e3, ' kN')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1.e-10 and abs(ratio2)<1.e-3  and abs(ratio3)<1.e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
