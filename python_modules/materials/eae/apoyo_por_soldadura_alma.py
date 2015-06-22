# -*- coding: utf-8 -*-
import math

def VdSoldAlma(l,a,bw,gmw,fu):
    '''
    Cortante último resistido por los cordones según el artículo 61.4.1
    de la instrucción EAE.}
    '''
    return l*2*a*bw*gmw*math.sqrt(3.0)*fu  #Revisar 30/11/2011

def LongMaxSoldAlma(tw):
    '''
    Longitud máxima de los los cordones según el artículo 61.4.1 de la instrucción EAE.
    '''
    return 14*tw

