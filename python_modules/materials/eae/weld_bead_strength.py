# -*- coding: utf-8 -*-

import scipy.interpolate
import math

def getTensComparacionCordonAnguloPG(sigmaN, tauN, tauPll):
  '''
  Returns the stress to use in weld bead strength checking according to article
   59.8.2 of EAE (page 256).

    :param sigmaN: Normal stress in weld bead throat plane (see figure 59.8).
    :param tauN: Tangential stress normal to weld bead axis
          in weld bead throat plane (see figure 59.8).
    :param tauPll: Tangential stress parallel to weld bead axis 
          in weld bead throat plane (see figure 59.8). 
  '''
  return math.sqrt((sigmaN)**2+3*((tauN)**2+(tauPll)**2))

# Table 59.8.2 in articl 59.8.2 of EAE (page 256)
x= [235e6,275e6,355e6,420e6,460e6]
y= [0.8,0.85,0.90,1.0,1.0]
tablaBetaW= scipy.interpolate.interp1d(x,y)

def getValorComparacionResistenciaCordon(fu, fy, gammaM2):
  '''
  Return weld bead ultimate stress to be used for strength checking 
   according to first condition in article 59.8.2 of EAE (page 256).

    :param fu: Tension strength of base material.
    :param fy: limit of elasticity of the welding steel (see table 59.8.2) 
        expressed in pascals.
    :param gammaM2: Partial safety factor of value 1.25. 
  '''
  return fu/tablaBetaW(fy)/gammaM2


def getFCCondicion1CordonPG(sigmaN, tauN, tauPll, fu, fy, gammaM2):
  '''
  Return the capacity factor for the first condition of the article
   59.8.2 de EAE (page 256).

    :param sigmaN: Normal stress in weld bead throat plane (see figure 59.8).
    :param tauN: Tangential stress normal to weld bead axis
          in weld bead throat plane (see figure 59.8).
    :param tauPll: Tangential stress parallel to weld bead axis 
          in weld bead throat plane (see figure 59.8). 

    :param fu: Tension strength of base material.
    :param fy: limit of elasticity of the welding steel (see table 59.8.2) 
        expressed in pascals.
    :param gammaM2: Partial safety factor of value 1.25. 
  '''
  return getTensComparacionCordonAnguloPG(sigmaN,tauN,tauPll)/getValorComparacionResistenciaCordon(fu,fy,gammaM2)

def getTensionNormalUltimaCordon(fu, gammaM2):
  '''
  Return the ultimate stress of the weld bead to be used for checking the
  second condition in the article 59.8.2 of EAE (page 256).

    :param fu: Tension strength of base material.
    :param fy: limit of elasticity of the welding steel (see table 59.8.2) 
        expressed in pascals.
    :param gammaM2: Partial safety factor of value 1.25. 
  '''
  return 0.9*fu/gammaM2

def getFCCondicion2CordonPG(sigmaN, fu, gammaM2):
  '''
  Return the capacity factor that correspond to the second condition of
  the article 59.8.2 of EAE (page 256).

    :param sigmaN: Normal stress in weld bead throat plane (see figure 59.8).
    :param fu: Tension strength of base material.
    :param gammaM2: Partial safety factor of value 1.25. 
  '''
  return abs(sigmaN)/getTensionNormalUltimaCordon(fu,gammaM2)

def getFCCordonPG(sigmaN, tauN, tauPll, fu, fy, gammaM2):
  '''
  Return the minimum of the capacity factors that correspond to the
  conditions of the article 59.8.2 of EAE (page 256).

  :param  sigmaN: Tensión normal sobre el plano de garganta del cordón (ver figura 59.8).
  :param  fu: Resistencia a tracción del acero de las piezas a soldar.
  :param  gammaM2: Partial safety factor of value 1.25.
  '''
  return min(getFCCondicion1CordonPG(sigmaN,tauN,tauPll,fu,fy,gammaM2),getFCCondicion2CordonPG(sigmaN,fu,gammaM2))


def getSigmaNPlanoGarganta(n, t):
  '''
  Return the normal stress in the throat plane from the stress values
  in the section rotated onto one of the weld legs.
  See "Estructuras de Acero" of Ramón Argüelles Álvarez 
  (url={https://books.google.es/books?id=ubIXPwAACAAJ}, isbn={9788495279972}) 
  page 4.17 and the book "Estructuras. Estructuras metálicas. U.D.2, Uniones"
  (url={https://books.google.es/books?id=X9JIRAAACAAJ}, isbn={9788486957087}) 
  of Vicente Cudós Samblancat from Escuela de la Edificación.

  :param  n: Normal stress over the rotated plane (see figure 24.1 
             on the book from UNED).
  :param  t: Tangential stress normal to the weld axis.
  '''
  return (n-t)/math.sqrt(2)

def getTauNPlanoGarganta(n, t):
  '''
  Return the tangential stress normal to the weld axis on the throat plane
  from the values of the stress in the section rotated onto one of the weld
  legs.
  See "Estructuras de Acero" of Ramón Argüelles Álvarez 
  (url={https://books.google.es/books?id=ubIXPwAACAAJ}, isbn={9788495279972}) 
  page 4.17 and the book "Estructuras. Estructuras metálicas. U.D.2, Uniones"
  (url={https://books.google.es/books?id=X9JIRAAACAAJ}, isbn={9788486957087}) 
  of Vicente Cudós Samblancat from Escuela de la Edificación.

  :param  n: Normal stress over the rotated plane (see figure 24.1 
             on the book from UNED).
  :param  t: Tangential stress normal to the weld axis.
'''
  return (n+t)/math.sqrt(2)

def getTensComparacionCordonAngulo(n, tn, ta):
  '''
  Return the stress to use in weld bead strength checking according to 
     the article 59.8.2 of EAE (page 256)

  :param  n: Normal stress over the rotated plane (see figure 24.1 
             on the book from UNED).
  :param  tn: Tangential stress over the rotated plane normal to the weld axis.
  :param  ta: Tangential stress over the rotated plane parallel to 
              the weld axis.
  '''
  sgN= getSigmaNPlanoGarganta(n,tn) 
  tN= getTauNPlanoGarganta(n,tn)
  return getTensComparacionCordonAnguloPG(sgN,tN,ta)

def getFCCondicion1Cordon(n, tn, ta, fu, fy, gammaM2):
  '''
  Return the capacity factor that correspond to the conditions
   of the article 59.8.2 of EAE (page 256)

  :param  n: Normal stress over the rotated plane (see figure 24.1 
             on the book from UNED).
  :param  tn: Tangential stress over the rotated plane normal to the weld axis.
  :param  ta: Tangential stress over the rotated plane parallel to 
              the weld axis.
  :param fu: Tension strength of base material.
  :param fy: limit of elasticity of the welding steel (see table 59.8.2) 
        expressed in pascals.
  :param gammaM2: Partial safety factor of value 1.25.
  '''
  return getTensComparacionCordonAngulo(n,tn,ta)/getValorComparacionResistenciaCordon(fu,fy,gammaM2)

def getFCCondicion2Cordon(n, tn, fu, gammaM2):
  '''
  Return the capacity factor that correspondt to the second condition of those
  in the article 59.8.2 of EAE (page 256)

  :param sigmaN: Normal stress in weld bead throat plane (see figure 59.8).
  :param fu: Tension strength of base material.
  :param gammaM2: Partial safety factor of value 1.25.
  '''
  sgN= getSigmaNPlanoGarganta(n,tn) 
  return abs(sgN)/getTensionNormalUltimaCordon(fu,gammaM2)
