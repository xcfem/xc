# -*- coding: utf-8 -*-
''' Check calculation of reinforcement under concentrated loads on solid 
    block members according to clause 61 of EHE-08.
'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2024, AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort@gmail.com l.pereztato@gmail.com"

import os
import math
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

# Materials
concrete= EHE_materials.HA30
steel= EHE_materials.B500S
# Load
Nd= 750e3

# Block member.
block= EHE_limit_state_checking.BlockMember(a= 0.25, a1= 0.15, b= 0.25, b1= 0.15, concrete= concrete, steel= steel)

# Compute maximum compressive strength (clause 61.2).
f3cd= block.getF3cd()
f3cdRef= -100e6/3.0
ratio1= abs(f3cd-f3cdRef)/f3cdRef

# Compute tie rods design tension (clause 61.3).
Td= block.getUad(Nd= Nd)
TdRef= Nd/10.0
ratio2= abs(Td-TdRef)/TdRef
# Compute required reinforcement area.
As= block.getReinforcementAreaAd(Nd= Nd)
numPhi10= int(math.ceil(As/EHE_materials.Fi10))
ratio3= (numPhi10-3)/3

'''
print('fc3d= '+'{:.2f}'.format(f3cd/1e6)+' MPa, ratio1= '+str(ratio1))
print('Td= '+'{:.2f}'.format(Td/1e3)+' kN ratio2= '+str(ratio2))
print('As= '+'{:.2f}'.format(As*1e4)+' cm2 '+str(numPhi10)+' ratio3= '+str(ratio3))
'''


fname= os.path.basename(__file__)
if((ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


