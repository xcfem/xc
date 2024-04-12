# -*- coding: utf-8 -*-
'''Snow loads according to IAPF.'''

from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO) "
__copyright__= "Copyright 2024,  LCPT A_OO "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import sys
from misc_utils import log_messages as lmsg
import scipy.interpolate
zi= [0, 200, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000]
zoneValues= dict()
zoneValues['I']= [0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.50e3, 0.60e3, 0.70e3, 1.10e3, 1.60e3, 1.80e3, 1.90e3, 2.20e3, 3.20e3, 3.80e3, 4.50e3, 5.30e3, 6.30e3, 7.40e3]
zoneValues['II']= [0.40e3, 0.40e3, 0.50e3, 0.60e3, 0.60e3, 0.70e3, 1.00e3, 1.10e3, 1.70e3, 1.90e3, 2.10e3, 2.40e3, 2.60e3, 3.60e3, 4.00e3, 4.50e3, 5.00e3, 5.60e3, 6.20e3]
zoneValues['III']= [0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.50e3, 0.60e3, 0.90e3, 1.00e3, 1.20e3, 1.40e3, 1.60e3, 2.20e3, 2.60e3, 3.00e3, 3.50e3, 4.10e3, 4.80e3]
zoneValues['IV']= [0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.40e3, 0.90e3, 1.00e3, 1.20e3, 1.40e3, 1.60e3, 2.20e3, 2.60e3, 3.00e3, 3.50e3, 4.10e3, 4.80e3]


zoneFunction= dict()
zoneFunction['I']= scipy.interpolate.interp1d(zi, zoneValues['I'], kind='linear')
zoneFunction['II']= scipy.interpolate.interp1d(zi, zoneValues['II'], kind='linear')
zoneFunction['III']= scipy.interpolate.interp1d(zi, zoneValues['III'], kind='linear')
zoneFunction['IV']= scipy.interpolate.interp1d(zi, zoneValues['IV'], kind='linear')

def get_snow_load_horizontal_terrain(z:float, climaticZone:int):
    ''' Return the value of the snow load in N/m2 according to table 2.8 of
        IAPF.

    :param z: height above sea level (m).
    :param climaticZone: climatic zone according to figure AN.1 of the spanish annex.
    '''
    retval= 4.6
    if(climaticZone==7):
        retval= 0.0
    elif(climaticZone in zoneFunction):
        retval= zoneFunction[climaticZone](z)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; unknown climatic zone: '+str(climaticZone))
    return retval

def get_snow_specific_weight(z:float):
    ''' Return the specific weight of the snow in N/m2 according to table 
        2.9 of IAPF.

    :param z: height above sea level (m).
    '''
    if(z<800):
        retval= 1.5e3
    elif(z<1000.0):
        retval= 2.0e3
    elif(z<1500.0):
        retval= 2.7e3
    elif(z<=2000.0):
        retval= 3.3e3
    else:
        retval= None
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; not implemented for heights above sea level greater than 1800 m')
    return retval
    
    
