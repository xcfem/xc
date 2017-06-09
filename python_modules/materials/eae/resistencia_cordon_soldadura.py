# -*- coding: utf-8 -*-

import scipy.interpolate
import math

'''
Devuelve la tensión de comparación a emplear en la comprobación de
   resistencia del cordón según el artículo 59.8.2 de EAE (página 256)
    sigmaN: Tensión normal sobre el plano de garganta del cordón (ver figura 59.8).
    tauN: Tensión tangencial perpendicular al eje de la soldadura
          sobre el plano de garganta del cordón (ver figura 59.8).
    tauPll: Tensión tangencial paralela al eje de la soldadura
          sobre el plano de garganta del cordón (ver figura 59.8). 
'''
def getTensComparacionCordonAnguloPG(sigmaN, tauN, tauPll):
    return math.sqrt((sigmaN)**2+3*((tauN)**2+(tauPll)**2))

# Tabla 59.8.2 del artículo 59.8.2 de EAE (página 256)
x= [235e6,275e6,355e6,420e6,460e6]
y= [0.8,0.85,0.90,1.0,1.0]
tablaBetaW= scipy.interpolate.interp1d(x,y)

'''
Devuelve la tensión de comparación última del cordón a emplear en la comprobación de
   resistencia del mismo según la primera condición del artículo 59.8.2 de EAE (página 256)
    fu: Resistencia a tracción del acero de las piezas a soldar.
    fy: Límite elástico del acero a soldar (ver tabla 59.8.2) expresado en pascales.
    gammaM2: Partial safety factor of value 1.25. 
'''
def getValorComparacionResistenciaCordon(fu, fy, gammaM2):
  return fu/tablaBetaW(fy)/gammaM2


'''
Devuelve el factor de capacidad correspondiente a la primera de las
   condiciones del artículo 59.8.2 de EAE (página 256)
    sigmaN: Tensión normal sobre el plano de garganta del cordón (ver figura 59.8).
    tauN: Tensión tangencial perpendicular al eje de la soldadura
          sobre el plano de garganta del cordón (ver figura 59.8).
    tauPll: Tensión tangencial paralela al eje de la soldadura
          sobre el plano de garganta del cordón (ver figura 59.8).
    fu: Resistencia a tracción del acero de las piezas a soldar.
    fy: Límite elástico del acero a soldar (ver tabla 59.8.2) expresado en pascales.
    gammaM2: Partial safety factor of value 1.25.
'''
def getFCCondicion1CordonPG(sigmaN, tauN, tauPll, fu, fy, gammaM2):
  return getTensComparacionCordonAnguloPG(sigmaN,tauN,tauPll)/getValorComparacionResistenciaCordon(fu,fy,gammaM2)

'''
Devuelve la tensión de comparación última del cordón a emplear en la comprobación de
   resistencia del mismo según la segunda condición del artículo 59.8.2 de EAE (página 256)
    fu: Resistencia a tracción del acero de las piezas a soldar.
    fy: Límite elástico del acero a soldar (ver tabla 59.8.2) expresado en pascales.
    gammaM2: Partial safety factor of value 1.25.
'''
def getTensionNormalUltimaCordon(fu, gammaM2):
  return 0.9*fu/gammaM2

'''
Devuelve el factor de capacidad correspondiente a la segunda de las
   condiciones del artículo 59.8.2 de EAE (página 256)
    sigmaN: Tensión normal sobre el plano de garganta del cordón (ver figura 59.8).
    fu: Resistencia a tracción del acero de las piezas a soldar.
    gammaM2: Partial safety factor of value 1.25.
'''
def getFCCondicion2CordonPG(sigmaN, fu, gammaM2):
    return abs(sigmaN)/getTensionNormalUltimaCordon(fu,gammaM2)

'''
Devuelve el mínimo de los factores de capacidad correspondientes a las
   condiciones del artículo 59.8.2 de EAE (página 256)
    sigmaN: Tensión normal sobre el plano de garganta del cordón (ver figura 59.8).
    fu: Resistencia a tracción del acero de las piezas a soldar.
    gammaM2: Partial safety factor of value 1.25.
'''
def getFCCordonPG(sigmaN, tauN, tauPll, fu, fy, gammaM2):
  return min(getFCCondicion1CordonPG(sigmaN,tauN,tauPll,fu,fy,gammaM2),getFCCondicion2CordonPG(sigmaN,fu,gammaM2))


'''
Devuelve la tensión normal en la sección por el plano de garganta a partir de
los valores de la tensión en la sección abatida sobre una de las caras de la soldadura.
Ver libro Estructuras de Acero de Ramón Argüelles Álvarez página 4.17 y
libro Uniones de Vicente Cudós Samblancat de la Escuela de la Edificación.
    n: Tensión normal sobre el plano abatido (ver figura 24.1 del libro de la UNED).
    t: Tensión tangencial normal al eje de la soldadura.
'''
def getSigmaNPlanoGarganta(n, t):
  return (n-t)/math.sqrt(2)

'''
Devuelve la tensión tangencial perpendicular al eje de soldadura en la sección por el
plano de garganta a partir de los valores de la tensión en la sección abatida sobre
una de las caras de la soldadura.
Ver libro Estructuras de Acero de Ramón Argüelles Álvarez página 4.17 y
libro Uniones de Vicente Cudós Samblancat de la Escuela de la Edificación.
    n: Tensión normal sobre el plano abatido (ver figura 24.1 del libro de la UNED).
    t: Tensión tangencial normal al eje de la soldadura.
'''
def getTauNPlanoGarganta(n, t):
  return (n+t)/math.sqrt(2)

'''
Devuelve la tensión de comparación a emplear en la comprobación de
   resistencia del cordón según el artículo 59.8.2 de EAE (página 256)
    n: Tensión normal sobre el plano de garganta abatido.
    tn: Tensión tangencial perpendicular al eje de la soldadura
          sobre el plano de garganta abatido.
    ta: Tensión tangencial paralela al eje de la soldadura
          sobre el plano de garganta abatido. 
'''
def getTensComparacionCordonAngulo(n, tn, ta):
  sgN= getSigmaNPlanoGarganta(n,tn) 
  tN= getTauNPlanoGarganta(n,tn)
  return getTensComparacionCordonAnguloPG(sgN,tN,ta)

'''
Devuelve el factor de capacidad correspondiente a la primera de las
   condiciones del artículo 59.8.2 de EAE (página 256)
    n: Tensión normal sobre el plano de garganta abatido.
    tn: Tensión tangencial perpendicular al eje de la soldadura
          sobre el plano de garganta abatido.
    ta: Tensión tangencial paralela al eje de la soldadura
          sobre el plano de garganta abatido.
    fu: Resistencia a tracción del acero de las piezas a soldar.
    fy: Límite elástico del acero a soldar (ver tabla 59.8.2) expresado en pascales.
    gammaM2: Partial safety factor of value 1.25.
'''
def getFCCondicion1Cordon(n, tn, ta, fu, fy, gammaM2):
  return getTensComparacionCordonAngulo(n,tn,ta)/getValorComparacionResistenciaCordon(fu,fy,gammaM2)

'''
Devuelve el factor de capacidad correspondiente a la segunda de las
   condiciones del artículo 59.8.2 de EAE (página 256)
    sigmaN: Tensión normal sobre el plano de garganta del cordón (ver figura 59.8).
    fu: Resistencia a tracción del acero de las piezas a soldar.
    gammaM2: Partial safety factor of value 1.25.
'''
def getFCCondicion2Cordon(n, tn, fu, gammaM2):
  sgN= getSigmaNPlanoGarganta(n,tn) 
  return abs(sgN)/getTensionNormalUltimaCordon(fu,gammaM2)
