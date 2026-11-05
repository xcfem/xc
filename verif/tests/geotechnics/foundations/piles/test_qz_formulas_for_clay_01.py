# -*- coding: utf-8 -*-
'''Trivial test for Matlock formulas.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from geotechnics.foundations import pytzqz_functions as pytzqzf

B = 1.0 # Pile diameter.
su= 85.0e3 # Undrained cohesion (Pa)
sigmaV= 250e3
sigmaV_prime= 150e3
porePressure= sigmaV-sigmaV_prime
L_trib = 1.0 # Tributary length.

qult, z50= pytzqzf._get_qz_param_clay(qzDepth= 14.0, su= su, sigV= sigmaV, b= B, drilledShaft= False)

# Check results.
err= max( abs(qult-1015e3), abs(z50-11.25e-3))

'''
print('tult= ', qult/1e3)
print('z50= ', z50)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
