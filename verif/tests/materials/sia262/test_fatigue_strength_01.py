# -*- coding: utf-8 -*-
from __future__ import print_function
''' Check fatigue strength according to clause 4.3.8.2.3 of SIA 262 2013.
'''

from materials.sia262 import SIA262_limit_state_checking

### Bridge deck transverse bottom reinforcement.
rebarDiam= 20e-3
rebarShape= 'straight'
DSigma_sd_D_trsv= SIA262_limit_state_checking.getSteelFatigueLimitStress(barShape= rebarShape, barDiameter= rebarDiam, overlappingConnectionsOnly= True)
ratio1= abs(DSigma_sd_D_trsv-116e6)/116e6

### Bridge deck longitudinal bottom reinforcement.
rebarDiam= 25e-3
rebarShape= 'straight'
DSigma_sd_D_long= SIA262_limit_state_checking.getSteelFatigueLimitStress(barShape= rebarShape, barDiameter= rebarDiam, overlappingConnectionsOnly= True)
ratio2= abs(DSigma_sd_D_long-96e6)/116e6

'''
print('Fatigue strength DSigma_sd_D= ', DSigma_sd_D_trsv/1e6, 'MPa')
print('Fatigue strength DSigma_sd_D= ', DSigma_sd_D_long/1e6, 'MPa')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) and(abs(ratio2)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
