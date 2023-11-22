# -*- coding: utf-8 -*-
'''Functions to compute wind loads according to Spanish IAPF standard.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
from actions.wind import IAP_wind
import scipy.interpolate

# Import common functions from IAP.
getTrapezoidalPressureDistribution= IAP_wind.getTrapezoidalPressureDistribution

def getRiskFactor(T:float, K= 0.2):
    ''' Return the probability factor according to clause 2.3.7.2 of IAPF-10.

    :param T: return period expressed in years.
    :param K: parameter (defaults to 0.2).
    '''
    return math.sqrt(0.562*(1.0-K*math.log(-math.log(1-1/T))))

def getSolidWebBridgeDragForceCoefficient(B:float, h_eq:float, webAngle:float= 0.0):
    ''' Return the wind drag force coefficient according to 
        clause 2.3.7.5.1 Caso 1) of IAPF-10.

    :param B: total width of the bridge deck.
    :param h_eq: effective height of the bridge deck.
    :param webAngle: web angle (radians).
    '''
    reductionCoef= max(0.7,1-(5/1000*math.degrees(webAngle)))
    retval= (2.5- 0.3*(B/h_eq))*reductionCoef
    return max(min(retval, 2.4), 1.3)

tb221a_B_h= [0.25, 0.33, 0.5, 0.67, 1.0, 1.5, 2.0,3,4,1e3] # B/h
tb221a_Cd= [2.1, 2.2, 2.2, 2.2, 2.0, 1.7, 1.4, 1.2, 1.1, 1.1] # dragg coefficient.
tb221a_CDInterp= scipy.interpolate.interp1d(tb221a_B_h, tb221a_Cd, fill_value= "extrapolate")

def getRectangularShapeDragCoefficient(B:float, h:float):
    ''' Return the drag coeffient for a rectangular shape according to figure 2.21 of IAPF-10.

    :param B: dimension of the rectangle parallel to the wind.
    :param h: dimension of the rectangle perpendicular to the wind.
    '''
    return tb221a_CDInterp(B/h)

def CdTableroAlmaLlenaIAPF(B,h,angAlma):
    '''CdTableroAlmaLlenaIAPF(B,h,angAlma)
    Coeficiente de arrastre para empuje horizontal sobre tableros
    de alma llena according to clause 2.3.7.5.1 IAPF.

    :param B: total width of the bridge deck.
    :param h: effective height of the bridge deck.
    :param angAlma: web angle (radians).
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; DEPRECATED, use getZ0.')
    return getSolidWebBridgeDragForceCoefficient(B= B, h_eq= h, webAngle= angAlma)

def getKz(terrainCategory:str):
    ''' Return the terrain factor according to table 2.4 of IAPF-10.

    :param terrainCategory: terrain category according to clause 2.3.7.2. 
                            of IAPF-10.
    '''
    retval= None
    if(terrainCategory=='0'):
        retval= .16
    elif(terrainCategory=='I'):
        retval= .17
    elif(terrainCategory=='II'):
        retval= .19
    elif(terrainCategory=='III'):
        retval= .22
    elif(terrainCategory=='IV'):
        retval= .23
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; terrainCategory: '+str(terrainCategory)+' unknown.')
    return retval

getZ0= IAP_wind.getZ0 # Return the rugosity length according to table 2.4 of IAPF-10.
getZMin= IAP_wind.getZmin # Return the minimum height according to table 2.4 of IAPF-10.

def getAlpha(terrainCategory):
    ''' Return the terrain factor according to table 2.4 of IAPF-10.

    :param terrainCategory: terrain category according to clause 2.3.7.2. 
                            of IAPF-10.
    '''
    retval= None
    if(terrainCategory=='0'):
        retval= .38
    elif(terrainCategory=='I'):
        retval= .44
    elif(terrainCategory=='II'):
        retval= .52
    elif(terrainCategory=='III'):
        retval= .61
    elif(terrainCategory=='IV'):
        retval= .67
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; terrainCategory: '+str(terrainCategory)+' unknown.')
    return retval

def getHeightFactor(terrainCategory:str, z):
    ''' Return the height factor (Cz) according to clause 2.3.7.2.
        of IAPF-10.

    :param terrainCategory: terrain category according to clause 2.3.7.2. 
                            of IAPF-10.
    :param z: height with respect to the terrain of the point for which the 
              speed is to be calculated.
    '''
    kz= getKz(terrainCategory)
    z0= getZ0(terrainCategory)
    zMin= getZMin(terrainCategory)
    retval= kz
    if(z>=zMin):
        retval*= math.log(z/z0)
    else:
        retval*= math.log(zMin/z0)
    return retval

def getGustFactor(terrainCategory:str, z:float, topographyFactor= 1.0):
    ''' Return the gust factor (Cg) according to clause 2.3.7.2.
        of IAPF-10.

    :param terrainCategory: terrain category according to clause 2.3.7.2. 
                            of IAPF-10.
    :param z: height with respect to the terrain of the point for which the 
              speed is to be calculated.
    '''
    kz= getKz(terrainCategory)
    Cz= getHeightFactor(terrainCategory= terrainCategory, z= z)
    return math.sqrt(1+7*kz/Cz/topographyFactor)

def getDesignVelocity(terrainCategory:str, v_ref: float, z:float, topographyFactor= 1.0, T= 100, K= 0.2):
    ''' Return the design velocity of the wind according to clause 2.3.7.2.
        of IAPF-10.

    :param terrainCategory: terrain category according to clause 2.3.7.2. 
                            of IAPF-10.
    :param v_ref: velocity of reference to figure 2.20 of IAPF-10.
    :param z: height with respect to the terrain of the point for which the 
              speed is to be calculated.
    :param topographyFactor: topography factor according to clause 2.3.7.2.
        of IAPF-10.
    :param T: return period expressed in years.
    :param K: parameter (defaults to 0.2).
    '''
    retval= topographyFactor*v_ref
    retval*= getRiskFactor(T= T, K= K) # risk factor.
    retval*= getHeightFactor(terrainCategory= terrainCategory, z= z) # height factor
    retval*= getGustFactor(terrainCategory= terrainCategory, z= z, topographyFactor= topographyFactor) # gust factor
    return retval

def getWindPressure(v_c: float, rho:float= 1.25):
    ''' Return the wind pressure according to clause 2.3.7.3. of IAPF-10.  

    :param v_c: design wind velocity according to clause 2.3.7.2. of IAPF-10.
    :param rho: density of air (defaults to 1.25 kg/m3).
    '''
    return 0.5*rho*v_c**2

def getUnitWindLoadDeck(terrainCategory:str, v_ref:float, z:float, Cd:float, topographyFactor= 1.0, T= 100, K= 0.2, rho:float= 1.25):
    ''' Return the wind load per square meter on a bridge member according 
        to clauses 2.3.7.3 of IAPF-10.

    :param terrainCategory: terrain category according to clause 2.3.7.2. 
                            of IAPF-10.
    :param v_ref: velocity of reference to figure 2.20 of IAPF-10.
    :param z: average height of the deck with respect to the terrain.
    :param Cd: drag force coefficient.
    :param topographyFactor: topography factor according to clause 2.3.7.2.
        of IAPF-10.
    :param T: return period expressed in years.
    :param K: parameter (defaults to 0.2).
    '''
    # Compute design velocity.
    v_c= getDesignVelocity(terrainCategory= terrainCategory, v_ref= v_ref, z= z, topographyFactor= topographyFactor, T= T, K= K)
    # Compute wind pressure.
    windPressure= getWindPressure(v_c= v_c, rho= rho)
    return Cd*windPressure
    
def ParamKIAPF(gae):
    '''ParamKIAPF(gae)
    Parámetro k en función del grado de aspereza del entorno
    according to IAPF standard (cuadro 2.4).
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; DEPRECATED, use getKz.')
    if gae==0:
        terrainCategory= '0'
    elif gae==1:
        terrainCategory= 'I'
    elif gae==2:
        terrainCategory= 'II'
    elif gae==3:
        terrainCategory= 'III'
    elif gae==4:
        terrainCategory= 'IV'
    else:
        lmsg.error(methodName+'; terrainCategory: '+str(gae)+' unknown.')
        terrainCategory= None
    return getKz(terrainCategory)

def ZminIAPF(gae):
    '''ZminIAPF(gae)
    Altura mínima z_min en función del grado de aspereza del entorno
    according to IAPF (cuadro 2.4).}
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; DEPRECATED, use getZmin.')
    if gae==0:
        terrainCategory= '0'
    elif gae==1:
        terrainCategory= 'I'
    elif gae==2:
        terrainCategory= 'II'
    elif gae==3:
        terrainCategory= 'III'
    elif gae==4:
        terrainCategory= 'IV'
    else:
        lmsg.error(methodName+'; terrainCategory: '+str(gae)+' unknown.')
        terrainCategory= None
    return getZMin(terrainCategory)

def Z0IAPF(gae):
    '''Z0IAPF(gae)
    Longitud de la rugosidad z_0 en función del grado de aspereza del entorno
    according to IAPF (cuadro 2.4).
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; DEPRECATED, use getZ0.')
    if gae==0:
        terrainCategory= '0'
    elif gae==1:
        terrainCategory= 'I'
    elif gae==2:
        terrainCategory= 'II'
    elif gae==3:
        terrainCategory= 'III'
    elif gae==4:
        terrainCategory= 'IV'
    else:
        lmsg.error(methodName+'; terrainCategory: '+str(gae)+' unknown.')
        terrainCategory= None
    return getZ0(terrainCategory)

def alphaIAPF(gae):
    '''alphaIAPF(gae)
    Valor del alpha en función del grado de aspereza del entorno
    according to IAPF (cuadro 2.4).}
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; DEPRECATED, use getAlpha.')
    if gae==0:
        terrainCategory= '0'
    elif gae==1:
        terrainCategory= 'I'
    elif gae==2:
        terrainCategory= 'II'
    elif gae==3:
        terrainCategory= 'III'
    elif gae==4:
        terrainCategory= 'IV'
    else:
        lmsg.error(methodName+'; terrainCategory: '+str(gae)+' unknown.')
        terrainCategory= None
    return getAlpha(terrainCategory)

def CzIAPF(gae,z):
    '''
    Heigth factor according to clause 2.3.7.2 of IAPF.
    '''
    kzCzIAPF=ParamKIAPF(gae)
    z0CzIAPF=Z0IAPF(gae)
    zminCzIAPF=ZminIAPF(gae)
    if z>zminCzIAPF:
        return kzCzIAPF*math.log(z/z0CzIAPF)
    else:
        return kzCzIAPF*math.log(zminCzIAPF/z0CzIAPF)
      
def CrIAPF(T):
    ''' Factor de riesgo en función del período de retorno T
    art. 2.3.7.2 IAPF
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; DEPRECATED, use getRiskFactor.')
    Cr=math.sqrt(0.562*(1-0.2*math.log(-math.log(1-1/T))))
    return Cr
 

def CgIAPF(kz,Cz,Ct):
    '''
    Factor de ráfaga according to clause 2.3.7.2 IAPF.
    '''
    return math.sqrt(1+(7*kz/Cz/Ct))
  


  

def FHkPorMetroIAPF(Cd,h,Vc):
    '''FHkPorMetroIAPF(Cd,h,Vc)
    Empuje horizontal per unit length sobre el tablero
    de alma llena according to clause 2.3.7.3 IAPF.
    '''
    rhoAire=1.25
    return Cd*h*0.5*rhoAire*Vc**2
  
def FVkPorMetroIAPF(b,Vc):
    '''FVkPorMetroIAPF(b,Vc)
    Empuje vertical per unit length sobre el tablero
    de alma llena according to clause 2.3.7.5.2 IAPF.
    '''
    rhoAire= 1.25
    return 0.5*b*0.5*rhoAire*Vc**2

    
descr_entorno={0:'mar o zona costera expuesta al mar abierto',
               1: 'lagos o áreas planas y horizontales con vegetación despreciable y sin obstáculos',
               2: 'zona rural con vegetación baja y obstáculos aislados (árboles, construcciones pequeñas, etc), con separaciones de al menos 20 veces la altura de los obstáculos.',
               3: 'zona suburbana, forestal o industrial con construcciones y obstáculos aislados con una separación máxima de 20 veces la altura de los obstáculos.',
               4: 'zona urbana en la que al menos el quince por ciento (15%) de la superficie esté edificada y la altura media de los edificios exceda de quince metros (15 m)'
               }

