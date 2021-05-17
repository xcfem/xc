# -*- coding: utf-8 -*-

import math
def coefRAsientoZapRectangularFlexible(l,b):
    '''
    coefRAsientoZapRectangularFlexible(l,b):
    Devuelve el coeficiente R de la expresión del asiento
    de una cimentación superficial flexible rectangular de
    acuerdo con el apartado 4.8.1 de la Guia de
    cimentaciones en obras de carretera, figura 4.10 de la página 113
    '''
    retv=b*math.asinh(l/float(b))+l*math.asinh(b/float(l))
    return(retv)


def asientoZapRectangularFlexible(L,B,p,E,nu):
    '''
    asientoZapRectangularFlexible(L,B,p,E,nu):
    Devuelve el asiento de una cimentación superficial flexible rectangular de
    acuerdo con el apartado 4.8.1 de la Guia de
    cimentaciones en obras de carretera, figura 4.10 de la página 113
    '''
    R=coefRAsientoZapRectangularFlexible(L,B)
    return(2.0/math.pi*R*p*(1.0-nu**2)/float(E))



