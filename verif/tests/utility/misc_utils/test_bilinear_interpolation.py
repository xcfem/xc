# -*- coding: utf-8 -*-
''' Test linear interpolation class.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np
from misc_utils import math_utils


bridgeType= [1, 2, 3] # bridge type 1 steel deck, 2 composite deck or 3 concrete deck
surfacingThickness= [0.0, .05, .1, .15, .75] # surfacing thickness (m).
## Recommended values of k sur to account for different surfacing thickness
### Top warmer than bottom.
kSurTopWarmerThanBottom=[[0.70, 0.90, 0.80],
                         [1.00, 1.00, 1.00],
                         [0.70, 1.00, 0.70],
                         [0.70, 1.00, 0.50],
                         [0.60, 0.80, 0.60]]

#### k_sur value interpolation for top warmer than bottom
fKSurTopWarmerThanBottom= math_utils.StaticGridBilinearInterpolator(surfacingThickness, bridgeType, kSurTopWarmerThanBottom)

def get_k_sur(bridgeType:int, surfacingDepth):
    ''' Return the recommended value of k_sur to account for different 
        surfacing thickness according to table 6.2 of EC1-1-5:2003.

    :param bridgeType: type of bridge: 1 steel deck, 2 composite deck or 3.1 concrete box girder or 3.2 concrete beam or 3.3 concrete slab.
    :param topWarmerThanBottom: true if the user requires the increment when the top surface is warmer than the deck bottom (deck heated), otherwise the bottom warmer than the top (deck cooled) is returned.
    :param surfacingDepth: depth of surfacing.
    :param ballast: true if the bridge deck is covered with ballast (0.75 m thick).
    :param waterproofedOnly: true if the bridge deck is only waterproofed with a dark color waterproofing layer.
    '''
    retval= fKSurTopWarmerThanBottom(np.array([surfacingDepth]), np.array([bridgeType]))
    retval= float(retval[0])
    return retval
err= 0.0
for j, bt in enumerate(bridgeType):
    for i, sd in enumerate(surfacingThickness):
        ks= get_k_sur(bt, sd)
        err+=(ks- kSurTopWarmerThanBottom[i][j])**2
        
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
