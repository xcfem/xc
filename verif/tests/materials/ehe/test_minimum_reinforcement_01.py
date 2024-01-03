# -*- coding: utf-8 -*-
''' Compute the minimum and maximum reiforcement amounts for a reinforced concret column member. Home made test.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from misc_utils import log_messages as lmsg

concrete= EHE_materials.HA35
steel= EHE_materials.B500S
Nd= -4830e3

diameter= 1.5

reinfRatios= EHE_limit_state_checking.ColumnReinforcementRatios(Ac= math.pi*(diameter/2.0)**2, concrete= concrete, reinforcingSteel= steel)

# Compute reinforcement areas.
asMinGeom= reinfRatios.getMinimumGeometricAmount()
asMinMech= reinfRatios.getMinimumMechanicalAmount(Nd= Nd)
asMax= reinfRatios.getMaximumReinforcementAmount()

# Check results.
ratio1= abs(asMinGeom-70.68583470577035e-4)/70.68583470577035e-4
ratio2= abs(asMinMech-12.075e-4)/12.075e-4
ratio3= abs(asMax-1030.8350894591508e-4)/1030.8350894591508e-4

'''
print('Minimum amount of reinforcement (geometric ratio): As= ', asMinGeom*1e4, ' cm2')
print("ratio1= ", ratio1)

print('Minimum amount of reinforcement (mechanical ratio): As= ', asMinMech*1e4, ' cm2')
print("ratio2= ", ratio2)

print('Maximum amount of reinforcement: As= ', asMax*1e4, ' cm2')
print("ratio3= ", ratio3)
'''

import os
fname= os.path.basename(__file__)
if ((abs(ratio1)<1e-4) and (abs(ratio2)<1e-4) and (abs(ratio3)<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
