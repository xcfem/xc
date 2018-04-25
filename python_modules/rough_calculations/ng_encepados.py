# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def getTracArmaduraInfEncepado(alpha,Nd):
    '''
    Returns the tension in the inferior reinforcement of the pile cap.

    :param alpha: Angle between the concrete compressed struts and the horizontal.
    :param Nd: Axil de cálculo en el pilote.
    '''
    return Nd/math.tan(alpha)
  

def getAreaNecArmaduraInfEncepado(alpha,Nd,fyd):
    '''
    Devuelve el área necesaria para la reinforcement inferior del encepado.

    :param alpha: Angle between the concrete compressed struts and the horizontal.
    :param Nd: Axil de cálculo en el pilote.
    '''
    return getTracArmaduraInfEncepado(alpha,Nd)/fyd
  

def getTracArmaduraInfEncepado2Pilotes(v,d,Nd):
    '''
    Devuelve la traccion en la reinforcement inferior de un encepado
    de DOS pilotes (ver números gordos HC.9 page 32).

    :param v: Distancia entre la pila y el pilote.
    :param d: pile cap effective depth.
    :param Nd: Axil de cálculo en la pila.
    '''
    return Nd*v/2/d
  

def getCantoMinimoEncepado(diam,D):
    '''
    Devuelve el depth mínimo del encepado.

    :param diam: Diámetro de la reinforcement del pilar o del pilote (la que sea mayor).
    :param D: Diámetro del pilote.
    '''
    return max((15*diam*diam)*100,max(D,0.4))
  

def getMinimalDistBetweenPileAxes(D):
    '''
    Return the minimal distance between pile axes.

    :param D: Diámetro del pilote.
    '''
    if D<0.5:
        return 2*D
    else:
        return 3*D
  

def getAreaArmaduraCercosVertEncepado2Pilotes(b,h,L):
    '''
    Devuelve el área de reinforcement necesaria para los cercos
    verticales de un encepado de DOS pilotes (ver números gordos
    HC.9 page 33).

    :param b: Ancho del encepado.
    :param h: Canto del encepado.
    :param L: Longitud del encepado.
    '''
    return 4*min(b,h/2)*L/1000
  

def getAreaArmaduraCercosHorizEncepado2Pilotes(b,h):
    '''
    Devuelve el área de reinforcement necesaria para los cercos
    horizontales de un encepado de DOS pilotes (ver números gordos
    HC.9 page 33).

    :param b: Ancho del encepado.
    :param L: Longitud del encepado.
    '''
    return 4*h*min(b,h/2)/1000
  
