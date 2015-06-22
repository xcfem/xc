# -*- coding: utf-8 -*-
from __future__ import division

#Armaduras de una ménsula corta según EHE-08.

def getCotgAnguloBiela(tipoJunta):
    '''getCotgAnguloBiela(tipoJunta)
    tipojunta: calidad de la junta de la ménsula con el pilar
    ("monolitica", "junta" o "junta_debil")

    Devuelve la cotangente del ángulo que forma la biela de compresión
    con la vertical según el apartado 64.1.2.1 de EHE-08.
    '''
    if tipoJunta=="monolitica":
        retval=1.4
    elif tipoJunta=="junta":
        retval=1.0
    elif tipoJunta=="junta_debil":
        retval=0.6
    return retval

def getCantoUtilMinimo(tipoJunta, a):
    '''getCantoUtilMinimo(tipoJunta, a)
    tipojunta: calidad de la junta de la ménsula con el pilar
    ("monolitica", "junta" o "junta_debil")
    a: Distancia (m) entre el eje de la carga aplicada y la sección
    de empotramiento de la ménsula (ver figura 64.1.2 de EHE-08).

    Devuelve el canto útil mínimo de la ménsula según
    el apartado 64.1.2.1 de EHE-08.
    '''
    return a*getCotgAnguloBiela(tipoJunta)/0.85

def getTraccionArmaduraPrincipal(tipoJunta, Fv,Fh):
    '''getTraccionArmaduraPrincipal(tipoJunta, Fv,Fh)
    tipojunta: calidad de la junta de la ménsula con el pilar
    ("monolitica", "junta" o "junta_debil")
    Fv: Carga vertical en la ménsula, positiva hacia abajo (N).
    Fh: Carga horizontal en la ménsula, positiva hacia afuera (N).

    Devuelve la tracción en la armadura principal de la ménsula según
    el apartado 64.1.2.1.1 de EHE-08.
    '''
    return Fv/getCotgAnguloBiela(tipoJunta)+Fh

def getAreaNecArmaduraPrincipal(tipoJunta, Fv,Fh,fyd):
    '''getAreaNecArmaduraPrincipal(tipoJunta, Fv,Fh,fyd)
    tipojunta: calidad de la junta de la ménsula con el pilar
    ("monolitica", "junta" o "junta_debil")
    Fv: Carga vertical en la ménsula, positiva hacia abajo (N).
    Fh: Carga horizontal en la ménsula, positiva hacia afuera (N).

    Devuelve el área necesaria para la armadura principal de la ménsula según
    el apartado 64.1.2.1.1 de EHE-08.
    '''
    return getTraccionArmaduraPrincipal(tipoJunta,Fv,Fh)/min(fyd,400e6)

def getTraccionCercos(Fv):
    ''' getTraccionCercos(Fv)
    Fv: Carga vertical en la ménsula, positiva hacia abajo (N).

    Devuelve la tracción en los cercos según el apartado 64.1.2.1.1 de EHE-08.
    '''
    return 0.2*Fv

def getAreaNecCercos(Fv,fyd):
    '''getAreaNecCercos(Fv,fyd)
    Fv: Carga vertical en la ménsula, positiva hacia abajo (N).
    fyd: Valor de cálculo de la resistencia del acero (Pa).

    Devuelve el área necesaria para los cercos de la ménsula según
    el apartado 64.1.2.1.1 de EHE-08.
    '''
    return getTraccionCercos(Fv)/min(fyd,400e6)

def getAreaNecApoyo(Fv,fcd):
    '''getAreaNecApoyo(Fv,fcd)
    Fv: Carga vertical en la ménsula, positiva hacia abajo (N).
    fcd: Valor de cálculo de la resistencia del hormigón (Pa).

    Devuelve el área necesaria para el apoyo según
    el apartado 64.1.2.1.2 de EHE-08.
    '''
    return Fv/0.7/fcd
