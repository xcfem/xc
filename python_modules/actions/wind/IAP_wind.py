# -*- coding: utf-8 -*-
'''Functions to compute wind loads according to Spanish IAP-11 code.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2021, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import scipy.interpolate
from misc_utils import log_messages as lmsg

# Probability factor

def getProbabilityFactor(T:float, K= 0.2, n= 0.5):
    ''' Return the probability factor according to clause 4.2.1 of IAP-11.

    :param T: return period expressed in years.
    :param K: parameter (defaults to 0.2).
    :param n: parameter (defaults to 0.5)
    '''
    return math.pow((1.0-K*math.log(-math.log(1-1/T)))/(1-K*math.log(-math.log(0.98))),n)

def getSolidWebBridgeDragForceCoefficient(B: float, h_eq: float):
    ''' Return the wind drag force coefficient according to 
        clause 4.2.5.1.1 a) of IAP-11.

    :param B: total width of the bridge deck.
    :param h_eq: effective height of the bridge deck.
    '''
    return max(min(2.5- 0.3*(B/h_eq), 2.4), 1.3)

fundamentalBasicWindSpeed= [26.0, 27.0, 29.0]
# Table 4.2-e
## Load on deck.
deckWindUnitLoad_10= dict()
deckWindUnitLoad_10["0"]= [2.58, 2.78, 3.21]
deckWindUnitLoad_10["I"]= [2.29, 2.47, 2.85]
deckWindUnitLoad_10["II"]= [1.94, 2.09, 2.41]
deckWindUnitLoad_10["III"]= [1.47, 1.58, 1.83]
deckWindUnitLoad_10["IV"]= [0.93, 1.00, 1.15]
## Load on piers.
pierWindUnitLoad_10= dict()
pierWindUnitLoad_10["0"]= [3.16, 3.40, 3.93]
pierWindUnitLoad_10["I"]= [2.79, 3.01, 3.47]
pierWindUnitLoad_10["II"]= [2.37, 2.56, 2.95]
pierWindUnitLoad_10["III"]= [1.80, 1.94, 2.23]
pierWindUnitLoad_10["IV"]= [1.14, 1.23, 1.42]

# Table 4.2-f
## Load on deck.
deckWindUnitLoad_20= dict()
deckWindUnitLoad_20["0"]= [2.93, 3.16, 3.65]
deckWindUnitLoad_20["I"]= [2.64, 2.85, 3.29]
deckWindUnitLoad_20["II"]= [2.31, 2.49, 2.88]
deckWindUnitLoad_20["III"]= [1.88, 2.03, 2.34]
deckWindUnitLoad_20["IV"]= [1.30, 1.40, 1.62]
## Load on piers.
pierWindUnitLoad_20= dict()
pierWindUnitLoad_20["0"]= [3.58, 3.86, 4.45]
pierWindUnitLoad_20["I"]= [3.23, 3.48, 4.02]
pierWindUnitLoad_20["II"]= [2.83, 3.05, 3.52]
pierWindUnitLoad_20["III"]= [2.29, 2.47, 2.85]
pierWindUnitLoad_20["IV"]= [1.60, 1.72, 1.99]

def getUnitWindLoadDeck(terrainCategory:str, v_b0: float, H_max:float= 20.0):
    ''' Return the wind load by square meter on bridge deck according to 
        table 4.2-e of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 of IAP-11.
    :param v_b0: fundamental basic wind velocity according to clause 4.2.1 of IAP-11.
    :param H_max: maximum height of the piers.
    '''
    unitLoads= None
    if(H_max<=10.0):
        unitLoads= deckWindUnitLoad_10[terrainCategory]
    elif(H_max<=10.0):
        unitLoads= deckWindUnitLoad_20[terrainCategory]
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; H_max= '+str(H_max)+' out of range (0,20] m.')
    unitWindLoad= scipy.interpolate.interp1d(fundamentalBasicWindSpeed, unitLoads, kind='linear')
    retval= None
    if((v_b0>=26.0) and (v_b0<=29)):
        retval= unitWindLoad(v_b0)*1e3
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; v_b0= '+str(v_b0)+' out of range [26,29] m/s.')
    return retval
    
def getUnitWindLoadPier(terrainCategory:str, v_b0: float, H_max:float= 20.0):
    ''' Return the wind load by square meter on bridge pier according to 
        table 4.2-e of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 of IAP-11.
    :param v_b0: fundamental basic wind velocity according to clause 4.2.1 of IAP-11.
    :param H_max: maximum height of the piers.
    '''
    unitLoads= None
    if(H_max<=10.0):
        unitLoads= pierWindUnitLoad_10[terrainCategory]
    elif(H_max<=10.0):
        unitLoads= pierWindUnitLoad_20[terrainCategory]
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; H_max= '+str(H_max)+' out of range (0,20] m.')
    unitWindLoad= scipy.interpolate.interp1d(fundamentalBasicWindSpeed, unitLoads, kind='linear')
    retval= None
    if((v_b0>=26.0) and (v_b0<=29)):
        retval= unitWindLoad(v_b0)*1e3
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; v_b0= '+str(v_b0)+' out of range [26,29] m/s.')
    return retval

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

