# -*- coding: utf-8 -*-
# Earthquake loads according to NCSE-02 (Spain).

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2021, A_OO   LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import math
import sys

def C(soilClass:str):
    ''' Return the value of soil factor according to clause 2.4 
                of NCSE-02.

    :param soilClass: soil classification (I, II, III or IV).
    '''
    retval= 1.4
    if(soilClass=='I'):
        retval= 1.0
    elif(soilClass== 'II'):
        retval= 1.3
    elif(soilClass== 'III'):
        retval= 1.6
    elif(soilClass== 'IV'):
        retval= 2.0
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+"; soil class: '"+soilClass+"' unknown.\n")
    return retval

def getSoilAmplificationFactorPGA(C:float, pga:float, rho: float):
    ''' Return the value of the soil amplification factor according to 
        the clause VI.2 of the publication "Actualización de mapas de 
        peligrosidad sísmica en España 2012"

    :param C: soil factor depending on the soil characteristics (clause 2.4 
              of NCSE-02.)
    :param pga: peak ground acceleration (clause VI.3 of the publication 
                "Actualización de mapas de peligrosidad sísmica en España 2012"
    :param rho: importance factor (clause 2.3 of NCSP-07 or clause 2.2 
                of NCSE-02).
    '''
    tmp= rho*pga
    g= 9.81
    retval= C
    if(tmp<=0.1*g):
        retval= C
    elif(tmp<=0.4*g):
        retval= 1+ 3.33*(1-C)((tmp/g) - 0.4)
    else:
        retval= 1.0
    return retval
