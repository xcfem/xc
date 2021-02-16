# -*- coding: utf-8 -*-
'''Functions to compute thermal loads according to Spanish IAP-11 code.'''

from __future__ import print_function
from __future__ import division


import math
import scipy.interpolate

# Table 4.3-a: Minimal annual temperature (Celsius degrees)
climateZone= [1, 2, 3, 4, 5, 6, 7] # climate zone according to figure 4.3-b
height= [0.0, 200.0, 400.0, 600.0, 800.0, 1000.0, 1200.0, 1400.0, 1600.0, 1800.0, 2000.0] # height

minAnnualTemp= [[-7.0, -11.0, -11.0, -6.0, -5.0, -6.0, 6.0],
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


fMinAnnualTemp= scipy.interpolate.interp2d(climateZone, height, minAnnualTemp, kind='linear')

def getMinAnnualTemp(climateZn: int, height: float, returnPeriod: float= 100):
    '''Return the minimal annual temperature according to table 4.3-a of IAP-11.

    :param climateZn: climate zone according to figure 4.3-a of IAP-11.
    :param height: height of the bridge location.
    :parem returnPeriod: return period of the temperature minimum in years.
    '''
    retval= float(fMinAnnualTemp(climateZn, height))
    if(returnPeriod!=50.0):
        k3= 0.393
        k4= -0.156
        p= 1.0/returnPeriod
        retval*= (k3+k4*math.log(-math.log(1-p)))
    return retval

