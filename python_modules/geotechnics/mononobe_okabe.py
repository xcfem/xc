# -*- coding: utf-8 -*-
'''Mononobe-Okabe theory.'''

from __future__ import division
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math

def theta_values(kh, kv):
    ''' Return the values of the auxiliary angle theta for the case of water
        table below the retaining wall in accordance with EN1998-5 
        equation (E.6).

    :param kh: horizontal seismic coefficient.
    :param kv: vertical seismic coefficient.
    '''
    v1= math.atan2(kh, 1-kv)
    v2= math.atan2(kh, 1+kv)
    return v1, v2

def active_earth_pressure_coefficient_Kad(psi, phi, theta, delta_ad,beta):
    ''' Return the active earth pressure coefficient according
        to Mononobe-Okabe according to EN1998-5 §E.4.

        :param psi: back face inclination of the structure (<= PI/2)
        :param phi: angle of internal friction of soil.
        :param theta: angle of weight with vertical:
           - kv= seismic coefficient of vertical acceleration.
           - kh= seismic coefficient of horizontal acceleration.
           - tan(theta)= kh/(1+kv) AND tan(theta)= kh/(1-kv)
        :param delta_ad: angle of friction soil - structure.
        :param beta: slope inclination of terrain.
    '''
    num= (math.sin(psi+phi-theta))**2
    denom1= math.cos(theta)*(math.sin(psi))**2*math.sin(psi-theta-delta_ad)
    if(beta<=theta):
        numA= math.sin(phi+delta_ad)*math.sin(phi-beta-theta)
        denomA= math.sin(psi-theta-delta_ad)*math.sin(psi+beta)
        denom2= (1+math.sqrt(numA/denomA))**2
        denom= denom1*denom2
    else:
        denom= denom1
    return num/denom
  
def passive_earth_pressure_coefficient_Kpd(psi, phi, theta, beta):
    ''' Return the passive earth pressure coefficient according
        to Mononobe-Okabe according to EN1998-5 §E.4.

        :param psi: back face inclination of the structure (<= PI/2)
        :param phi: angle of internal friction of soil.
        :param theta: angle of weight with vertical:
           - kv= seismic coefficient of vertical acceleration.
           - kh= seismic coefficient of horizontal acceleration.
           - tan(theta)= kh/(1+kv) AND tan(theta)= kh/(1-kv)
        :param beta: slope inclination of terrain.'''
    num= (math.sin(psi+phi-theta))**2
    denom1= math.cos(theta)*(math.sin(psi))**2*math.sin(psi+theta)
    numA= math.sin(phi)*math.sin(phi+beta-theta)
    denomA= math.sin(psi+theta)*math.sin(psi+beta)
    denom2= (1-math.sqrt(numA/denomA))**2
    denom= denom1*denom2
    return num/denom

def overpressure_dry(H, gamma_soil, kv, kh, psi, phi, delta_ad, beta, Kas):
    ''' Overpressure due to seismic action according to Mononobe-Okabe

        :param H: height of the structure.
        :param gamma_soil: unit weight of soil (backfill)
        :param kv: seismic coefficient of vertical acceleration.
        :param kh: seismic coefficient of horizontal acceleration.
        :param psi: back face inclination of the structure (<= PI/2)
        :param phi: angle of internal friction of soil.
        :param delta_ad: angle of friction soil - structure.
        :param beta: slope inclination of backfill.
        :param Kas: static earth pressure coefficient 
    '''
    theta= math.atan(kh/(1+kv)) #angle of weight with vertical:
                                #tan(theta)= kh/(1+kv) AND tan(theta)= kh/(1-kv)
    K_ad= active_earth_pressure_coefficient_Kad(psi= psi, phi= phi, theta= theta, delta_ad= delta_ad, beta= beta)
    minuendo= (1+kv)*K_ad
    factor= minuendo-Kas
    if(factor<0.0):
        methodName= sys._getframe(0).f_code.co_name
        errMsg= methodName+'; negative factor in Mononobe-Okabe overpressure.'
        errMsg+= '\n Active earth pressure coefficient. K_ad= '+str(K_ad) 
        errMsg+= '\n Static earth pressure coefficient. Kas: '+str(Kas) 
        errMsg+= '\n Seismic coefficient of horizontal acceleration. kh: '+str(kh) 
        lmsg.error(errMsg)
        
    return 0.5*gamma_soil*H**2*factor
