# -*- coding: utf-8 -*-
''' Check rectangular hollow section class.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

from materials.sections import section_properties as sp

box= sp.RectangularHollowSection(name= 'test', h= 40e-3, b= 20e-3, t= 1e-3)
J= box.getJTorsion()
Jref= 18933.8275862069*(1e-3)**4

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
