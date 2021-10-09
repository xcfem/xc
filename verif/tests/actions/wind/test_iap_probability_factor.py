# -*- coding: utf-8 -*-
''' Test computation of hiding ratios according to table 4.2-c of IAP-11.'''

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

'''
print("ratio1= ",ratio1)
''' 


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
