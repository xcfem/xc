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

def getWindPressure(v_b: float, rho:float= 1.25):
    ''' Return the wind pressure according to clause 4.2.3 of IAP-11.  

    :param v_b: basic wind velocity according to clause 4.2.1 of IAP-11.
    :param rho: density of air (defaults to 1.25 kg/m3).
    '''
    return 0.5*rho*v_b**2

def getZmin(terrainCategory:str):
    ''' Return the minimum height according to table 4.2-b of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    '''
    retval= None
    if(terrainCategory in ['0', 'I']):
        retval= 1.0
    elif(terrainCategory=='II'):
        retval= 2.0
    elif(terrainCategory=='III'):
        retval= 5.0
    elif(terrainCategory=='IV'):
        retval= 10.0
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; terrainCategory: '+str(terrainCategory)+' unknown.')
    return retval

def getZ0(terrainCategory:str):
    ''' Return the rugosity length according to table 4.2-b of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    '''
    retval= None
    if(terrainCategory=='0'):
        retval= .003
    elif(terrainCategory=='I'):
        retval= .01
    elif(terrainCategory=='II'):
        retval= .05
    elif(terrainCategory=='III'):
        retval= .3
    elif(terrainCategory=='IV'):
        retval= 1.0
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; terrainCategory: '+str(terrainCategory)+' unknown.')
    return retval

def getKr(terrainCategory:str):
    ''' Return the terrain factor according to table 4.2-b of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    '''
    retval= None
    if(terrainCategory=='0'):
        retval= .156
    elif(terrainCategory=='I'):
        retval= .17
    elif(terrainCategory=='II'):
        retval= .19
    elif(terrainCategory=='III'):
        retval= .216
    elif(terrainCategory=='IV'):
        retval= .235
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; terrainCategory: '+str(terrainCategory)+' unknown.')
    return retval
        

def getWindExpositionFactor(terrainCategory:str, z:float, c_0:float= 1.0, k_l: float= 1.0):
    ''' Return the value of the wind exposition factor according to clause
        4.2.3 of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    :param z: Height above ground surface at the site of the building or 
              other structure.
    :param k_l: turbulent factor (defaults to 1.0) according to 
                clause 4.2.2 of IAP-11.
    :param c_0: topography factor (defaults to 1.0) according to 
                clause 4.2.2 of IAP-11.
    '''
    z_min= getZmin(terrainCategory)
    z= max(z, z_min)
    z_0= getZ0(terrainCategory)
    k_r= getKr(terrainCategory)
    ln= math.log(z/z_0)
    return k_r**2*(c_0**2*ln**2+7*k_l*c_0*ln)
        
def getVerticalPressure(terrainCategory:str, z:float, v_b:float, c_0:float= 1.0, k_l: float= 1.0, c_fz= (-0.9,0.9), rho= 1.25):
    ''' Return the value of the average vertical pressure according to clause
        4.2.5.1.2 of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    :param z: Height above ground surface at the site of the building or 
              other structure.
    :param v_b: basic wind velocity according to clause 4.2.1 of IAP-11.
    :param k_l: turbulent factor (defaults to 1.0) according to 
                clause 4.2.2 of IAP-11.
    :param c_0: topography factor (defaults to 1.0) according to 
                clause 4.2.2 of IAP-11.
    :param c_fz: force coefficient in vertical direction 
                 (defaults to (-0.9,0.9)).
    :param rho: density of air (defaults to 1.25 kg/m3).
    '''
    windPressure= getWindPressure(v_b, rho)
    c_e= getWindExpositionFactor(terrainCategory, z, c_0, k_l)
    return (windPressure*c_e*c_fz[0], windPressure*c_e*c_fz[1])

def getVerticalForce(terrainCategory:str, z:float, v_b:float, A_refz:float, c_0:float= 1.0, k_l: float= 1.0, c_fz= (-0.9,0.9), rho= 1.25):
    ''' Return the value of the vertical force according to clause
        4.2.5.1.2 of IAP-11.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    :param z: Height above ground surface at the site of the building or 
              other structure.
    :param v_b: basic wind velocity according to clause 4.2.1 of IAP-11.
    :param A_refz: area of the bridge deck (horizontal projection).
    :param k_l: turbulent factor (defaults to 1.0) according to 
                clause 4.2.2 of IAP-11.
    :param c_0: topography factor (defaults to 1.0) according to 
                clause 4.2.2 of IAP-11.
    :param c_fz: force coefficient in vertical direction 
                 (defaults to (-0.9,0.9)).
    :param rho: density of air (defaults to 1.25 kg/m3).
    '''
    vp= getVerticalPressure(terrainCategory, z, v_b, c_0, k_l, c_fz, rho)
    return (vp[0]*A_refz, vp[1]*A_refz)

#          a
#      --------->|                 1    |                
#       -------->/             ========>|     -
#        ------->/                      |     ^        
#         ------>/   h    <>            |     |    
#          ----->/                      |     | hR
#           ---->/                      |     |
#            --->/                      |     -
#              b

def getLinearDistribution(h:float, hR:float):
    ''' Return the values (a,b) of a linear pressure distribution whose
        resultant passes through a point at heigth= hR.

    :param h: height of the pressure distribution.
    :param hR: height of the resultant.
    '''
    ratio= hR/h
    a= 2*(3*ratio-1)
    b= 2*(2-3*ratio)
    return (a,b)

def getVerticalPressureDistribution(terrainCategory:str, x0: float, x1: float, z:float, v_b:float, c_0:float= 1.0, k_l: float= 1.0, c_fz= (-0.9,0.9), rho= 1.25):
    ''' Return the vertical pressure distribution over a bridge deck to fullfill
    the clause 4.2.5.1.3 of IAP-11. The resultant of the pressure passes at a
    distance of the leftmost point of the deck equal to a quarter of the
    deck width.

    :param terrainCategory: terrain category according to clause 4.2.2 
                            of IAP-11.
    :param x0: leftmost point of the deck.
    :param x1: rigthmost point of the deck.
    :param z: Height above ground surface at the site of the building or 
              other structure.
    :param v_b: basic wind velocity according to clause 4.2.1 of IAP-11.
    :param k_l: turbulent factor (defaults to 1.0) according to 
                clause 4.2.2 of IAP-11.
    :param c_0: topography factor (defaults to 1.0) according to 
                clause 4.2.2 of IAP-11.
    :param c_fz: force coefficient in vertical direction 
                 (defaults to (-0.9,0.9)).
    :param rho: density of air (defaults to 1.25 kg/m3).
    '''
    averagePressure= getVerticalPressure(terrainCategory, z, v_b, c_0, k_l, c_fz, rho)
    w= x1-x0 # Deck width
    (a,b)= getLinearDistribution(h= w, hR= 0.75*w)
    xi= [x0, x1]
    y0i= [averagePressure[0]*a, averagePressure[0]*b]
    pressureDistrib0= scipy.interpolate.interp1d(xi, y0i, kind='linear')
    y1i= [averagePressure[1]*a, averagePressure[1]*b]
    pressureDistrib1= scipy.interpolate.interp1d(xi, y1i, kind='linear')
    return (pressureDistrib0, pressureDistrib1)


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

#           a
#     ------------>|                           |
#       ---------->|                           |
#         -------->|     <=>    ==============>| point of application.
#           ------>| h                         |
#             ---->|                           |
#              --->|                           |
#               -->|                           |
#                b

def getTrapezoidalPressureDistribution(h:float, heightFraction:float= 0.6, averagePressure= 1.0):
    ''' Return the dimensions of the parallel sides for the trapezoidal pressure
        over the lateral surface of the bridge such as the resultant height is
        0.6*h over the bottom edge of the surface.

    :param h: height of the surfacd loaded by the wind.
    :param pointOfApplicationHeight: fraction of the surface height of the 
     desired point of application (defaults to 0.6 according to 
     clause 4.2.5.1.3 of IAP-11).
    :param averagePressure: average value of the wind pressure. 
    '''
    (topSide, bottomSide)= getLinearDistribution(h= h, hR= h*heightFraction) 
    return scipy.interpolate.interp1d([0.0,h], [bottomSide, topSide], kind='linear')


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

