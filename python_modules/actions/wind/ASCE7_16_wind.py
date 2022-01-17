# -*- coding: utf-8 -*-
''' Wind according to ASCE 7-16. 
Only the wind parameters whose evaluation
have changed in ASCE 7-22 are included in this module. These parameters
are those involved in the calculation of the velocity pressure coefficient Kz.
 All the other calculations can be performed by the routines included in module 
ASCE7_wind.py, according to ASCE7-22, since they have not changed between 
ASCE 7-16 and ASCE 7-22'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, AO_O and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import math

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
        to note 1 of table 26.10-1 of ASCE 7-16. This parameter depends 
        on the height above ground level of the point where the wind 
        pressure is considered, and the exposure category. 

    :param exposure: 'B', 'C' or 'D'
    :param z: Height above ground surface at the site of the building or 
              other structure.
    '''
    zgC= zgConstant(exposure)
    alphaC= alphaConstant(exposure)
    zv= max(z,4.6)
    return 2.01*math.pow(zv/zgC,2.0/alphaC)


