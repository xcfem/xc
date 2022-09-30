# -*- coding: utf-8 -*-
''' Slipstream effect according to EC1-2 clause 6.6.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import sys
from misc_utils import log_messages as lmsg


def q1k(v:float, a_g:float):
    '''Characteristic value of q_1k according to figure 6.22 of EC1-2
    (analytic expression taken from the Spanish national annex).

    :param v: speed of the train (m/s).
    :param a_g: distance from the track axis to the surface of the structure (m).
    '''
    if(a_g<2.3):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; distance from the track axis: '+str(a_g) + ' out of range. Expression is valid for distances greater than 2.3 m')
    
    return (2.5/((a_g+0.25)**2)+0.02)*v**2/1.6

def q2k(v:float, h_g:float):
    '''Characteristic value of q_2k according to figure 6.23 of EC1-2
    (analytic expression taken from the Spanish national annex).

    :param v: speed of the train (m/s).
    :param h_g: distance from top of rail level to the underside of the structure (m).
    '''
    
    return (2.0/((h_g-3.1)**2)+0.015)*v**2/1.6

def q3k(v:float, a_g:float):
    '''Characteristic value of q_3k according to figure 6.24 of EC1-2
    (analytic expression taken from the Spanish national annex).

    :param v: speed of the train (m/s).
    :param a_g: distance from the track axis to the surface of the structure (m).
    '''
    
    return (1.5/((a_g+0.25)**2)+0.015)*v**2/1.6
