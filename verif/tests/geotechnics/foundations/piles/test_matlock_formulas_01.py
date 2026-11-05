# -*- coding: utf-8 -*-
'''Trivial test for Matlock formulas.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from geotechnics.foundations import pytzqz_functions as pytzqzf

cu= 40.0e3 # Undrained cohesion (Pa)
gamma= 18.0e3 # Unit weight of the soil (N/m³)
z = 3.5 # Depth of the spring.
B = 0.8 # Pile diameter.
L_trib = 1.0 # Tributary length.
sigV= gamma*z
pult= pytzqzf._get_pult_matlock(pyDepth= z, cu= cu, sigV= sigV, b= B, tribLength= L_trib)
y50= pytzqzf._get_y_50_matlock(cu= cu, b= B)

# Check results.
err= max( abs(pult-216.4e3), abs(y50-.04))

# print('pult= ', pult/1e3)
# print('y50= ', y50)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
