# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math

def longElasticaPiloteArcillas(Epilote,Ipilote,Eterreno,L):
    '''
    longElasticaPiloteArcillas(Epilote,Ipilote,Eterreno,L):
    Elastic length of the pile in clay
    '''
    LePA=math.pow(Epilote*Ipilote/(Eterreno/3.),0.25)
    if LePA>2*L:
        print("Pile is too short to compute its elastic length.")
    return(LePA)

def longEmpotramientoPiloteArcillas(Epilote,Ipilote,Eterreno,L):
    '''
    longEmpotramientoPiloteArcillas(Epilote,Ipilote,Eterreno,L): 
    Anchorage length of the pile in clay
    '''
    return(1.2*longElasticaPiloteArcillas(Epilote,Ipilote,Eterreno,L))
