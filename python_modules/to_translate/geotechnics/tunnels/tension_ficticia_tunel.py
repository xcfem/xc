# -*- coding: utf-8 -*-
'''Cálculo de la presión radial ficticia según el método de Panet y Guenot
según el documento «Tema VIII: Cálculo de túneles por elementos finitos» de
Francisco Javier Castanedo Navarro.'''

def calcNumeroEstab(sigma0,sigmaC):
  ''' Devuelve el valor del número de estabilidad a partir de:
    sigma0: Tensión geostática.
    sigmaC: Resistencia a compresión del macizo.
    El número de estabilidad se interpreta como:
    N=1: Túnel en estado elástico.
    N= 2.75: Comienza la plastificación del túnel.
    N= 4: Frente con problemas.
    N= 6: Frente inestable'''
  return 2*sigma0/sigmaC

# Points that define lambda0 in function of the stability number.
ptosLambda0= 
\ptosLambda0
  {
    geom.Pos2d(0,0.265)
    geom.Pos2d(1,0.265)
    geom.Pos2d(3,0.58)
  }

'''Points that define the stability number "N" in function of the
   ratio between the excavation radius and the plastification radius.'''
ptosNumeroEstab= 
\ptosNumeroEstab
  {
    geom.Pos2d(1,1)
    geom.Pos2d(1.125,1.5)
    geom.Pos2d(1.45,2)
    geom.Pos2d(1.75,2.5)
    geom.Pos2d(2.15,3)
  }

''' Devuelve el valor del número de estabilidad a partir de los valores de:

    r: radius of the excavation.
    rp: plastification radius.'''
def calcNumeroEstabFromRp(r,rp):
  return ptosNumeroEstab.valor(rp/r)

'''Points that define the ratio between the excavation radius and the
   plastification radius from the number of stability.'''
ptosPlastificationRadius= 
\ptosPlastificationRadius
  {
    geom.Pos2d(1,1)
    geom.Pos2d(1.5,1.125)
    geom.Pos2d(2,1.45)
    geom.Pos2d(2.5,1.75)
    geom.Pos2d(3,2.15)
  }

def calcPlastificationRadius(r,N):
  ''' Return the plastification radius from the values of:

      r: radius of the excavation
      N: Número de estabilidad.'''
  return ptosPlastificationRadius.valor(N)*r

def calcLambda0(r,rp):
  ''' Devuelve el valor de lambda0 (parámetro que introduce el efecto del grado de plastificación del frente) a partir de:
    r: radius of the excavation
    rp: plastification radius.'''
  return ptosLambda0.valor(calcNumeroEstabFromRp(r,rp))

''' Return the value of m (parameter that depends on the effect of the 
    distance to the front and of the radius of plastification) from the 
    number of stability.

    x: distance to the excavation front.
    r: radius of the excavation.
    rp: radius of plastification.'''
def calcM(x,r,rp):
  mNeq1= 1-exp(-x/0.7*r)
  mNeq3= 1-pow(1+x/0.84/rp,-2)
  return interpolateLine(1,mNeq1,3,mNeq3,calcNumeroEstabFromRp(r,rp))

''' Return the value of lambda (variation of the pressure with the distance to 
    the front) from:

    lambda0: Parámetro que introduce el efecto del grado de plastificación del frente.
    x: distance to the excavation front.
    r: radius of the excavation.
    rp: radius of plastification.'''
def calcLambda(x,r,rp):
  lambda0= calcLambda0(r,rp)
  return lambda0+(1-lambda0)*calcM(x,r,rp)

''' Devuelve el módulo de la presión radial ficticia
    a partir de:
    sigma0: Tensión inicial geostática.
    x: distance to the excavation front.
    r: radius of the excavation.
    rp: radius of plastification.'''
def sigmaRPanetGuenot(sigma0,x,r,rp):
  return (1-calcLambda(x,r,rp))*sigma0

