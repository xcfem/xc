# -*- coding: utf-8 -*-
''' Test calculation of anchorage lengths according to EHE.
    Home made test.'''

from __future__ import division
from __future__ import print_function

import math
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
import os
from misc_utils import log_messages as lmsg
from collections import OrderedDict
from pyexcel_ods import save_data

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Materials
ha25= EHE_materials.HA25
b500s= EHE_materials.B500S
b400s= EHE_materials.B400S
diam= 8e-3
cCover= .05
lbIHA25B500= {0.006:0.15, 0.008:0.2,0.01:0.25,0.012:0.3,0.016:0.4,0.02:0.6, 0.025:0.938}
lbIIHA25B500= {0.006:0.214, 0.008:0.286, 0.01:0.357, 0.012:0.429, 0.016:0.571, 0.02:0.84, 0.025:1.313}
lbIHA25B400= {0.006:0.12, 0.008:0.16, 0.01:0.2, 0.012:0.24, 0.016:0.32, 0.02:0.48, 0.025:0.75}
lbIIHA25B400= {0.006:0.1714, 0.008:0.2286, 0.01:0.2857, 0.012:0.3429, 0.016:0.4571, 0.02:0.672, 0.025:1.05}

# Rebar controller.
rebarControllerPosI= EHE_limit_state_checking.RebarController(concreteCover= cCover, pos= 'I')
rebarControllerPosII= EHE_limit_state_checking.RebarController(concreteCover= cCover, pos= 'II')


err= 0.0
for key in lbIHA25B500:
    lbRef= lbIHA25B500[key]
    lbI= rebarControllerPosI.getNetAnchorageLength(ha25, key, b500s, beta= 1.0, efficiency= 1.0)
    err+= (lbRef-lbI)**2
    
for key in lbIIHA25B500:
    lbRef= lbIIHA25B500[key]
    lbI= rebarControllerPosII.getNetAnchorageLength(ha25, key, b500s, beta= 1.0, efficiency= 1.0)
    err+= (lbRef-lbI)**2
    
for key in lbIHA25B400:
    lbRef= lbIHA25B400[key]
    lbI= rebarControllerPosI.getNetAnchorageLength(ha25, key, b400s, beta= 1.0, efficiency= 1.0)
    err+= (lbRef-lbI)**2
    
for key in lbIIHA25B400:
    lbRef= lbIIHA25B400[key]
    lbI= rebarControllerPosII.getNetAnchorageLength(ha25, key, b400s, beta= 1.0, efficiency= 1.0)
    err+= (lbRef-lbI)**2

err= math.sqrt(err)
#print(err)

import os
fname= os.path.basename(__file__)
if(err<.05):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
