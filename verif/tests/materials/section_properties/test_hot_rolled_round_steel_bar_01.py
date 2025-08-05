# -*- coding: utf-8 -*-
''' Check rectangular hollow section class.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import math
from materials.ec3 import EC3_materials

#Materials
## Steel material
steel= EC3_materials.S275JR
steel.gammaM= 1.00

## Steel sections
testSection= EC3_materials.RShape(steel= steel, name= 'R32')
J= testSection.J()

r= 32e-3/2.0
Jref= math.pi*r**4/2

ratio1= abs(J-Jref)/Jref

'''
print("J= ",J)
print("Jref= ",Jref)
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<2e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
