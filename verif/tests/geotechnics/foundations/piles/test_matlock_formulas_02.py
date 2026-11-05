# -*- coding: utf-8 -*-
'''Trivial test for Matlock formulas.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from geotechnics.foundations import pytzqz_functions as pytzqzf

gamma= 19.86e3 # Unit weight of the soil (N/m³)
pileDiameter= 1.0 # Pile diameter.
L_trib = 1.0 # Tributary length.

# Soil at depth 9.0
depth_1= 9.0
cu_1= 20e3
sigV_1= gamma*depth_1
pult_1= pytzqzf._get_pult_matlock(pyDepth= depth_1, cu= cu_1, sigV= sigV_1, b= pileDiameter, tribLength= 1.0)
y50_1= pytzqzf._get_y_50_matlock(cu= cu_1, b= pileDiameter)

# Soil at depth 45.0
depth_2= 45.0
cu_2= 35e3
sigV_2= gamma*depth_2
pult_2= pytzqzf._get_pult_matlock(pyDepth= depth_2, cu= cu_2, sigV= sigV_2, b= pileDiameter, tribLength= 1.0)
y50_2= pytzqzf._get_y_50_matlock(cu= cu_2, b= pileDiameter)

# Check results.
err_1= max( abs(pult_1-180e3), abs(y50_1-.05))
err_2= max( abs(pult_2-315e3), abs(y50_1-.05))
err= max(err_1, err_2)

'''
print('pult_1= ', pult_1/1e3)
print('y50_1= ', y50_1)
print('err_1= ', err_1)
print('pult_2= ', pult_2/1e3)
print('y50_2= ', y50_2)
print('err_2= ', err_2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
