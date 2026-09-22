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

climateZone= [1, 2, 3, 4, 5, 6, 7] 
height= [0.0, 200.0, 400.0, 600.0, 800.0, 1000.0, 1200.0, 1400.0, 1600.0, 1800.0, 2000.0]
minAnnualShadeAirTemp= [[-7.0, -11.0, -11.0, -6.0, -5.0, -6.0, 6.0],
                        [-10.0, -13.0, -12.0, -8.0, -8.0, -8.0, 5.0],
                        [-12.0, -15.0, -14.0, -10.0, -11.0, -9.0, 3.0],
                        [-15.0, -16.0, -15.0, -12.0, -14.0, -11.0, 2.0],
                        [-18.0, -18.0, -17.0, -14.0, -17.0, -13.0, 0.0],
                        [-20.0, -20.0, -19.0, -16.0, -20.0, -14.0, -2.0],
                        [-23.0, -21.0, -20.0, -18.0, -23.0, -16.0, -3.0],
                        [-26.0, -23.0, -22.0, -20.0, -26.0, -17.0, -5.0],
                        [-28.0, -25.0, -23.0, -22.0, -29.0, -19.0, -7.0],
                        [-31.0, -26.0, -25.0, -24.0, -32.0, -21.0, -8.0],
                        [-33.0, -28.0, -27.0, -26.0, -35.0, -22.0, -10.0]]

# Minimum shade air temperature interpolation.
fMinAnnualShadeAirTemp= math_utils.StaticGridBilinearInterpolator(height, climateZone, minAnnualShadeAirTemp)

def get_min_annual_shade_air_temp(climateZn: int, height: float):
    ''' Return the minimal annual shade air temperature for a return period 
        of 50 years according to table AN.1 of the Spanish National Annex.

    :param climateZn: climate zone according to figure AN.2 of the Spanish National Annes (identical to figure 4.3-b of IAP-11).
    :param height: height of the bridge location.
    '''
    retval= fMinAnnualShadeAirTemp(np.array([height]), np.array([climateZn]))
    retval= float(retval[0])
    return retval

err= 0.0
for j, cz in enumerate(climateZone):
    for i, h in enumerate(height):
        t= get_min_annual_shade_air_temp(cz, h)
        err+=(t- minAnnualShadeAirTemp[i][j])**2
        
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
