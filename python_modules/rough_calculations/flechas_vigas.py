# -*- coding: utf-8 -*-

from __future__ import division

#See notations in XC users manual

def getFlechaVigaBiapQUnif(l,EI,q):
    '''
    Devuelve la flecha en el centro de vano de una viga biapoyada
    bajo carga uniforme
    l: Luz entre apoyos.
    EI: Rigidez a flexión.
    q: Carga por unidad de longitud (uniforme).
    '''  
    return -5*q*l**3/384/EI
  
def deflCantBeamPconcentr(l,EI,P,a):
    '''
    Maximum deflection in a cantilever beam with a concentrated load at any point
    '''
    return P*a**2/6/EI*(3*l-a)

def deflCantBeamQunif(l,EI,q):
    '''
    Maximum deflection in a cantilever beam with a uniformly distributed load
    '''
    return q*l**4/8/EI

def deflCantBeamMend(l,EI,M):
    '''
    Maximum deflection in a cantilever beam with a couple moment at the free end
    '''
    return M*l**2/2/EI

def deflSimplSupBeamPconcentr(l,EI,P,b):
    '''
    Maximum deflection in a beam simply supported at ends with a concentrated load at any point
    '''
    return P*b*(l**2-b**2)**(3/2)/9/3**(1/2)/l/EI

def deflSimplSupBeamQunif(l,EI,q):
    '''
    Maximum deflection in a beam simply supported at ends with a uniformly distributed load
    '''
    return 5*q*l**4/384/EI

def deflSimplSupBeamMend(l,EI,M):
    '''
    Maximum deflection in a beam simply supported at ends with a couple moment at the right end
    '''
    return M*l**2/9/3**(1/2)/EI


