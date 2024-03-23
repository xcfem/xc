# -*- coding: utf-8 -*-
''' Compute the wind force coefficient on a circular cylinder according to
the expressions in the figurea 7.28 of EN 1991-1-4:2005.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
from actions.wind import ec1_wind

# Compute the value of the force coefficient without free-end flow.
b= 4
dtot= 2
cfz_2_0= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= 0.0, beta= 0.0)
cfz_2_0Ref= 0.6642857142857143
ratio= list()
ratio.append(abs(cfz_2_0-cfz_2_0Ref)/cfz_2_0Ref)

cfz_2_3= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= math.radians(3), beta= 0.0)
cfz_2_3Ref= 0.7139610389610389
ratio.append(abs(cfz_2_3-cfz_2_3Ref)/cfz_2_3Ref)

cfz_2_6= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= math.radians(6), beta= 0.0)
cfz_2_6Ref= 0.7636363636363637
ratio.append(abs(cfz_2_6-cfz_2_6Ref)/cfz_2_6Ref)

cfz_2_8= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= math.radians(8), beta= 0.0)
cfz_2_8Ref= 0.7943181818181818
ratio.append(abs(cfz_2_8-cfz_2_8Ref)/cfz_2_8Ref)

cfz_2_10= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= math.radians(10), beta= 0.0)
cfz_2_10Ref= 0.825
ratio.append(abs(cfz_2_10-cfz_2_10Ref)/cfz_2_10Ref)

b= 40.0
cfz_20_0= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= 0.0, beta= 0.0)
cfz_20_0Ref= 0.15
ratio.append(abs(cfz_20_0-cfz_20_0Ref)/cfz_20_0Ref)

cfz_20_3= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= math.radians(3), beta= 0.0)
cfz_20_3Ref= 0.5181818181818182
ratio.append(abs(cfz_20_3-cfz_20_3Ref)/cfz_20_3Ref)

cfz_20_6= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= math.radians(6), beta= 0.0)
cfz_20_6Ref= 0.8863636363636364
ratio.append(abs(cfz_20_6-cfz_20_6Ref)/cfz_20_6Ref)

cfz_20_8= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= math.radians(8), beta= 0.0)
cfz_20_8Ref= 0.8931818181818182
ratio.append(abs(cfz_20_8-cfz_20_8Ref)/cfz_20_8Ref)

cfz_20_10= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= b, dtotVP= dtot, alpha= math.radians(10), beta= 0.0)
cfz_20_10Ref= 0.9
ratio.append(abs(cfz_20_10-cfz_20_10Ref)/cfz_20_10Ref)

rMax= max(ratio)
rMin= min(ratio)
testOK= max(rMax,abs(rMin))<1e-4
'''
print("cfz_2_0= ", cfz_2_0)
print("ratio[0]= ", ratio[0])
print("cfz_2_3= ", cfz_2_3)
print("ratio[1]= ", ratio[1])
print("cfz_2_6= ", cfz_2_6)
print("ratio[2]= ", ratio[2])
print("cfz_2_8= ", cfz_2_8)
print("ratio[3]= ", ratio[3])
print("cfz_2_10= ", cfz_2_10)
print("ratio[4]= ", ratio[4])
print("cfz_20_0= ", cfz_20_0)
print("ratio[5]= ", ratio[5])
print("cfz_20_3= ", cfz_20_3)
print("ratio[6]= ", ratio[6])
print("cfz_20_6= ", cfz_20_6)
print("ratio[7]= ", ratio[7])
print("cfz_20_8= ", cfz_20_8)
print("ratio[8]= ", ratio[8])
print("cfz_20_10= ", cfz_20_10)
print("ratio[9]= ", ratio[9])
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
