# -*- coding: utf-8 -*-
''' Trivial test for the eta_05_0123 module.

ETA-05/0123. Bonded, unbonded, and external post-tensioning kits for 
prestressing of structures with bars, diameter 17,5 - 47 mm
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.eota import eta_05_0123
import math

test_values= [438e3, 638e3, 808e3, 997e3, 219e3, 499e3, 722e3, 912e3, 1131e3, 1566e3, 722e3, 912e3]


err= 0.0
for tv, bar in zip(test_values, [eta_05_0123.bar_26E, eta_05_0123.bar_32E, eta_05_0123.bar_36E, eta_05_0123.bar_40E, eta_05_0123.bar_18WR, eta_05_0123.bar_26WR, eta_05_0123.bar_32WR, eta_05_0123.bar_36WR, eta_05_0123.bar_40WR, eta_05_0123.bar_47WR, eta_05_0123.bar_32WS, eta_05_0123.bar_36WS]):
    err+= (tv-bar.getMaxOverstressingForce())
err= math.sqrt(err)

# print(err)
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
