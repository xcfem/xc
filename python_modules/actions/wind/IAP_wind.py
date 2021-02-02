# -*- coding: utf-8 -*-
'''Functions to compute wind loads according to Spanish IAP-11 code.'''

from __future__ import print_function
from __future__ import division


import math
import scipy.interpolate

srHidingFactor= [0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 1e3] # solidity ratio
rsHidingFactor= [0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0] # relative spacement

zHidingFactor= [[0.75, 0.40, 0.31, 0.22, 0.13, 0.06, 0.06],
[1.00, 0.82, 0.64, 0.46, 0.28, 0.10, 0.10],
[1.00, 0.84, 0.68, 0.52, 0.36, 0.20, 0.20],
[1.00, 0.86, 0.72, 0.59, 0.45, 0.31, 0.31],
[1.00, 0.89, 0.78, 0.68, 0.57, 0.46, 0.46],
[1.00, 1.00, 0.92, 0.85, 0.77, 0.69, 0.69],
[1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00]] # hiding factor

fHidingFactor= scipy.interpolate.interp2d(srHidingFactor, rsHidingFactor, zHidingFactor, kind='linear')

def getHidingFactor(relativeSpacement: float, solidityRatio: float):
    '''Return the hiding factor of a surface with respect to other
       according to table 4.2-c of IAP-11.

    :param relativeSpacement: relative spacement according to 
                              clause 4.2.3 of IAP-11.
    :param solidityRatio: solidity ratio  according to clause 4.2.3 of IAP-11.
    '''
    return float(fHidingFactor(solidityRatio, relativeSpacement))

