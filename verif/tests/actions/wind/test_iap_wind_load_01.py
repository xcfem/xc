# -*- coding: utf-8 -*-
''' Miscelaneous test of wind loads according to IAP-11.'''

from __future__ import print_function
from __future__ import division

from actions.wind import IAP_wind

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

c_prob= IAP_wind.getProbabilityFactor(T= 100.0, K= 0.2, n= 0.5)
ratio1= abs(c_prob-1.0384765480213738)/1.0384765480213738
c_fx= IAP_wind.getSolidWebBridgeDragForceCoefficient(B= 3.8, h_eq= 1.6)
ratio2= abs(c_fx-1.7875)/1.7875
f_wx= IAP_wind.getUnitWindLoadDeck(terrainCategory='III', v_b0= 29, H_max= 5.0)
ratio3= abs(f_wx-1.83e3)/1.83e3


'''
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
''' 


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) and (abs(ratio2)<1e-12) and (abs(ratio3)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
