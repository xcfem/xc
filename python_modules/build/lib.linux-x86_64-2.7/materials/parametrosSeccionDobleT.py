# -*- coding: utf-8 -*-

from __future__ import division
##   Parámetros que definen la sección doble T.
##
##                        b1
##             ---------------------- ef
##                       |
##                       |
##                       |
##                       |ew
##                       |
##                       |
##                 ------------- ef
##                      b2
##   siendo:
##     -b1: Ancho del ala superior.
##     -b2: Ancho del ala inferior.
##     -ef: Espesor de las alas.
##     -ew: Espesor del alma.
##     -ht: Canto del perfil.
   

def getInerciaTorsionSeccDobleT(b1,b2,ef,ew,ht):
    '''
    Devuelve el valor de la inercia a torsión de una sección doble T
    monocelular según el artículo «Perfil doble T» de la Wikipedia.
    -b1: Ancho del ala superior.
    -b2: Ancho del ala inferior.
    -ef: Espesor de las alas.
    -ew: Espesor del alma.
    -ht: Canto del perfil.
    '''
    hPrf=ht-ef
    return ((b1+b2)*ef**3+hPrf*ew**3)/3
  

def getMomentoAlabeoSeccDobleT(b1,b2,ef,ew,ht):
    '''
    Devuelve el momento de alabeo de una sección doble T
    monocelular según el artículo «Perfil doble T» de la Wikipedia.
    -b1: Ancho del ala superior.
    -b2: Ancho del ala inferior.
    -ef: Espesor de las alas.
    -ew: Espesor del alma.
    -ht: Canto del perfil.
     '''
    hPrf=ht-ef
    return ef*hPrf**2/12*b1**3*b2**3/(b1**3+b2**3)
  

def getMomentoModuloTorsionSeccDobleT(b1,b2,ef,ew,ht):
    '''
    Devuelve el momento resistente de una sección doble T
    monocelular según el artículo «Perfil doble T» de la Wikipedia.
    -b1: Ancho del ala superior.
    -b2: Ancho del ala inferior.
    -ef: Espesor de las alas.
    -ew: Espesor del alma.
    -ht: Canto del perfil.
     '''
    J=getInerciaTorsionSeccDobleT(b1,b2,ef,ew,ht)
    return J/max(ef,ew)
  

