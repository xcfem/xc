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

def ParamKIAPF(gae):
    '''ParamKIAPF(gae)
    Parámetro k en función del grado de aspereza del entorno
    according to IAPF standard (cuadro 2.4).
    '''
    if gae==0:
          return 0.16
    elif gae==1:
          return 0.17
    elif gae==2:
          return 0.19
    elif gae==3:
          return 0.22
    elif gae==4:
          return 0.23
    else:
        print("Error en el grado de aspereza del entorno.")
        return 0.0

def ZminIAPF(gae):
    '''ZminIAPF(gae)
    Altura mínima z_min en función del grado de aspereza del entorno
    according to IAPF (cuadro 2.4).}
    '''
    if gae==0:
          return 1
    elif gae==1:
          return 1
    elif gae==2:
          return 2
    elif gae==3:
          return 5
    elif gae==4:
          return 10
    else:
         print("Error en el grado de aspereza del entorno.")
         return 0.0

def Z0IAPF(gae):
    '''Z0IAPF(gae)
    Longitud de la rugosidad z_0 en función del grado de aspereza del entorno
    according to IAPF (cuadro 2.4).
    '''
    if gae==0:
          return 0.003
    elif gae==1:
          return 0.01
    elif gae==2:
          return 0.05
    elif gae==3:
          return 0.3
    elif gae==4:
          return 1
    else:
        print("Error en el grado de aspereza del entorno.")
        return 0.0

def alphaIAPF(gae):
    '''alphaIAPF(gae)
    Valor del alpha en función del grado de aspereza del entorno
    according to IAPF (cuadro 2.4).}
    '''
    if gae==0:
          return 0.38
    elif gae==1:
          return 0.44
    elif gae==2:
          return 0.52
    elif gae==3:
          return 0.61
    elif gae==4:
          return 0.67
    else:
        print("Error en el grado de aspereza del entorno.")
        return 0.0

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
      
  

def CgIAPF(kz,Cz,Ct):
    '''
    Factor de ráfaga according to clause 2.3.7.2 IAPF.
    '''
    return math.sqrt(1+(7*kz/Cz/Ct))
  

def CdTableroAlmaLlenaIAPF(B,h,angAlma):
    '''CdTableroAlmaLlenaIAPF(B,h,angAlma)
    Coeficiente de arrastre para empuje horizontal sobre tableros
    de alma llena according to clause 2.3.7.5.1 IAPF.
    '''
    coefRedCdIAPF= max(0.7,1-(5/1000*math.degrees(angAlma)))
    tmpCdIAPF=2.5-0.3*B/h
    tmpCdIAPF= max(tmpCdIAPF,0.3)
    tmpCdIAPF= min(tmpCdIAPF,2.4)*coefRedCdIAPF
    return tmpCdIAPF
  

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

