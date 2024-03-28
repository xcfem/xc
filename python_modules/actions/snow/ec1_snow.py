# -*- coding: utf-8 -*-
'''Snow loads according to Eurocode 1 part 1-3.'''

from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO) "
__copyright__= "Copyright 2024,  LCPT A_OO "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import sys
from misc_utils import log_messages as lmsg
import scipy.interpolate

zi= [0, 200, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800]

zoneValues= dict()
zoneValues[1]= [0.3e3, 0.5e3, 0.6e3, 0.7e3, 0.9e3, 1e3, 1.2e3, 1.4e3, 1.7e3, 2.3e3, 3.2e3, 4.3e3, 4.3e3]
zoneValues[2]= [0.4e3, 0.5e3, 0.6e3, 0.7e3, 0.9e3, 1e3, 1.1e3, 1.3e3, 1.5e3, 2e3, 2.6e3, 3.5e3, 4.6e3]
zoneValues[3]= [0.2e3, 0.2e3, 0.2e3, 0.3e3, 0.3e3, 0.4e3, 0.5e3, 0.6e3, 0.7e3, 1.1e3, 1.7e3, 2.6e3, 4e3]
zoneValues[4]= [0.2e3, 0.2e3, 0.3e3, 0.4e3, 0.5e3, 0.6e3, 0.8e3, 1e3, 1.2e3, 1.9e3, 3e3, 4.6e3, 4.6e3]
zoneValues[5]= [0.2e3, 0.3e3, 0.4e3, 0.4e3, 0.5e3, 0.6e3, 0.7e3, 0.8e3, 0.9e3, 1.3e3, 1.8e3, 2.5e3, 2.5e3]
zoneValues[6]= [0.2e3, 0.2e3, 0.2e3, 0.3e3, 0.4e3, 0.5e3, 0.7e3, 0.9e3, 1.2e3, 2e3, 3.3e3, 4.3e3, 4.3e3]
# zoneValues[7]= [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
zoneFunction= dict()
zoneFunction[1]= scipy.interpolate.interp1d(zi, zoneValues[1], kind='linear')
zoneFunction[2]= scipy.interpolate.interp1d(zi, zoneValues[2], kind='linear')
zoneFunction[3]= scipy.interpolate.interp1d(zi, zoneValues[3], kind='linear')
zoneFunction[4]= scipy.interpolate.interp1d(zi, zoneValues[4], kind='linear')
zoneFunction[5]= scipy.interpolate.interp1d(zi, zoneValues[5], kind='linear')
zoneFunction[6]= scipy.interpolate.interp1d(zi, zoneValues[6], kind='linear')
# zoneFunction[7]= scipy.interpolate.interp1d(zi, zoneValues[7], kind='linear')


def get_snow_load_horizontal_terrain(z:float, climaticZone:int):
    ''' Return the value of the snow load in N/m2 according to table AN.2 of
        the Spanish annex.

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
        AN.1 of the Spanish annex.

    :param z: height above sea level (m).
    '''
    if(z<800):
        retval= 1.5e3
    elif(z<1000.0):
        retval= 2.0e3
    elif(z<1500.0):
        retval= 2.7e3
    elif(z<=1800.0):
        retval= 3.3e3
    else:
        retval= None
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; not implemented for heights above sea level greater than 1800 m')
    return retval
    
    
