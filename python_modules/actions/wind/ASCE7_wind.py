# -*- coding: utf-8 -*-
''' Wind according to SIA 261'''

from __future__ import division
from __future__ import print_function

import math
import scipy.interpolate

xk1= [0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5]
k12DRidge= [0.29, 0.36, 0.43, 0.51, 0.58, 0.65, 0.72]
k12DRidgeInterp= scipy.interpolate.interp1d(xk1,k12DRidge)
k12DEscarpment= [0.17, 0.21, 0.26, 0.3, 0.34, 0.38, 0.43]
k12DEscarpmentInterp= scipy.interpolate.interp1d(xk1,k12DEscarpment)
k13DAxisymmetricalHill= [0.21, 0.26, 0.32, 0.37, 0.42, 0.47, 0.53]
k13DAxisymmetricalHillInterp= scipy.interpolate.interp1d(xk1,k13DAxisymmetricalHill)

def K1K2K3(siteCondition: str, H: float, Lh: float, x: float, upWindOfCrest: bool, z: float):
    ''' Return K1, K2 and K3 factors according to figure 26.8.1 of ASCE 7-16.

    :param siteCondition: site condition ('2DR':2D ridge, '2DE': 2D escarpment,
                          '3DA': 3D axisymmetrical hill).
    :param H: Height of hill or escarpment relative to the upwind terrain.
    :param Lh: Distance upwind of crest to where the difference in ground 
               elevation is half the height of hill or escarpment.
    :param x: Distance (upwind or downwind) from the crest to the site of 
              the building or other structure.
    :param upWindOfCrest: true if the building is upwind from the crest,
                          false otherwise.
    :param z: Height above ground surface at the site of the building or 
              other structure.
    '''
    H_Lh= min(H/Lh, 0.5)
    gamma= 3.0 # height attenuation factor.
    nu= 1.5 # horizontal attenuation factor.
    k1= 0.72 # Factor to account for shape of topographic feature and maximum 
             # speed-up effect according to figure 26.8.1 of ASCE 7-16.
    if(siteCondition=='2DR'):
        k1= float(k12DRidgeInterp(H_Lh))
    elif(siteCondition=='2DE'):
        k1= float(k12DEscarpmentInterp(H_Lh))
        gamma= 2.5
        if(not upWindOfCrest):
            nu= 4.0 
    elif(siteContition=='3DA'):
        k1= float(k13DAxisymmetricalHillInterp(H_Lh))
        gamma= 4.0
    k2= 1.0-abs(x)/nu*Lh # factor to account for reduction in speed-up with
                         # distance upwind or downwind of crest.
    k3= math.exp(-gamma*z/Lh) # factor to account for reduction in speed-up
                              # with height above local terrain
    return k1*k2*k3

def Kzt(siteCondition: str, H: float, Lh: float, x: float, upWindOfCrest: bool, z: float):
    ''' Return K1, K2 and K3 factors according to figure 26.8.1 of ASCE 7-16.

    :param siteCondition: site condition ('2DR':2D ridge, '2DE': 2D escarpment,
                          '3DA': 3D axisymmetrical hill).
    :param H: Height of hill or escarpment relative to the upwind terrain.
    :param Lh: Distance upwind of crest to where the difference in ground 
               elevation is half the height of hill or escarpment.
    :param x: Distance (upwind or downwind) from the crest to the site of 
              the building or other structure.
    :param upWindOfCrest: true if the building is upwind from the crest,
                          false otherwise.
    :param z: Height above ground surface at the site of the building or 
              other structure.
    '''
    k1k2k3= K1K2K3(siteCondition, H, Lh, x, upWindOfCrest, z)
    return (1+k1k2k3)**2

def Ke(zg):
    ''' Ground elevation factor Ke according to expression note 2 of 
        table 26.9-1 of ASCE 7-16.

    :param zg: ground elevation above sea level.
    '''
    return math.exp(-0.000119*zg)

def alphaConstant(exposure: str):
    ''' Return the terrain exposure constant alpha
        according to table 26.11-1 of ASCE 7-16.

    :param exposure: 'B', 'C' or 'D'
    '''
    if(exposure=='B'):
        return 7.0
    elif(exposure=='C'):
        return 9.5
    elif(exposure=='D'):
        return 11.5
    else:
        lmsg.error('exposure value: \''+exposure+'\' unknown.')

def zgConstant(exposure: str):
    ''' Return the terrain exposure constant zg
        according to table 26.11-1 of ASCE 7-16.

    :param exposure: 'B', 'C' or 'D'
    '''
    if(exposure=='B'):
        return 365.76
    elif(exposure=='C'):
        return 274.32
    elif(exposure=='D'):
        return 213.36
    else:
        lmsg.error('exposure value: \''+exposure+'\' unknown.')

def Kz(exposure: str, z: float):
    ''' Return velocity pressure coefficient Kz and Kh according
        to note 1 of table 26.10-1 of ASCE 7-16.

    :param exposure: 'B', 'C' or 'D'
    :param z: Height above ground surface at the site of the building or 
              other structure.
    '''
    zgC= zgConstant(exposure)
    alphaC= alphaConstant(exposure)
    zv= max(z,4.6)
    return 2.01*math.pow(zv/zgC,2.0/alphaC)


def qz(z, Kz, Kzt, Kd, zg, V):
    ''' Velocity pressure according to expression 26.10-1.si
        of ASCE 7-16.

    :param Kz: velocity pressure exposure coefﬁcient, according to 
               section 26.10.1 of ASCE 7-16.
    :param Kzt: topographic factor, according to 
               section 26.8.2 of ASCE 7-16.
    :param Kd: wind directionality factor, according to 
               section 26.6 of ASCE 7-16.
    :param zg: ground elevation above sea level.
    :param V: basic wind speed, , according to 
               section 26.5 of ASCE 7-16.
    '''
    return 0.613*Kz*Kzt*Kd*Ke(zg)*V**2 
