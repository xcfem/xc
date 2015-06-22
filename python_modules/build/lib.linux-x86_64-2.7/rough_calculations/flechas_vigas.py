# -*- coding: utf-8 -*-

from __future__ import division
def getFlechaVigaBiapQUnif(l,EI,q):
    '''
    Devuelve la flecha en el centro de vano de una viga biapoyada
    bajo carga uniforme
    l: Luz entre apoyos.
    EI: Rigidez a flexión.
    q: Carga por unidad de longitud (uniforme).
    '''  
    return -5*q*l**3/384/EI
  
