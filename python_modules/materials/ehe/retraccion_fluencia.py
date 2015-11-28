# -*- coding: utf-8 -*-

from __future__ import division
from materials.ehe import auxEHE
import math

# Fluencia según EHE-08 (Artº 39.8).

'''
Devuelve el valor de la función beta(t0) que interviene en el cálculo
   del coeficiente básico de fluencia representando la influencia de la
   edad de puesta en carga del hormigón.
   t0: Instante a partir del cual actúa la carga que produce la fluencia
       expresado en días.
  ¡OJO! En la EHE-08 de diciembre de 2008 esta fórmula está mal
  (cambia la suma del denominador por una resta). 
'''
def getBetaT0(t0):
  return 1.0/(0.1+pow(t0,0.2)) 

'''
Devuelve el valor de la función beta(fck)
   que interviene en el cálculo del coeficiente básico de fluencia
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getBetaFcm(fck):
  return 16.8/math.sqrt(fck/1e6+8) 

'''
Función auxiliar para la obtención de los coeficientes alpha_1,
   alpha_2 y alpha_3 (ver página 176 abajo).
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getAlphaFluencia(fcm, exponente):
  return pow(35e6/fcm,exponente) 

'''
Devuelve el valor del coeficiente alpha_1 (ver página 176 abajo)
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getAlpha1Fluencia(fck):
  return getAlphaFluencia(fck+8e6,0.7) 

'''
Devuelve el valor del coeficiente alpha_2 (ver página 176 abajo)
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getAlpha2Fluencia(fck):
  return getAlphaFluencia(fck+8e6,0.2) 

'''
Devuelve el valor del coeficiente alpha_3 (ver página 176 abajo)
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getAlpha3Fluencia(fck):
  return getAlphaFluencia(fck+8e6,0.5) 

'''
 Devuelve el valor del coeficiente de influencia de la humedad
    relativa phi_HR(HR,e,fcm)
    que interviene en el cálculo del coeficiente básico de fluencia
    HR: Humedad relativa del aire expresada en tanto por uno.
     u: Perímetro de la pieza en contacto con la atmósfera expresado en metros.
     A: Área de la sección transversal de la pieza expresada en metros.
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getPhiHR(HR, u, A, fck):
    retval= 0.0
    tMp= (1-HR)/(0.1*pow((2*A/u)*1000,1/3))
    fcm= fck+8e6
    if(fcm<=35E6):
      retval= 1+tMp
    else:
      alPha1= getAlpha1Fluencia(fck)
      alPha2= getAlpha2Fluencia(fck)
      retval= (1+tMp*alPha1)*alPha2
    return retval

'''
Devuelve el valor del coeficiente básico de fluencia phi_0(fck,HR,e)
   a partir de:
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
   t0: Instante a partir del cual actúa la carga que produce la fluencia expresado en días.
  HR: Humedad relativa del aire expresada en tanto por uno.
   u: Perímetro de la pieza en contacto con la atmósfera expresado en metros.
   A: Área de la sección transversal de la pieza expresada en metros.
'''
def getPhi0(fck, t0, HR, u, A):
  return getPhiHR(HR,u,A,fck)*getBetaFcm(fck)*getBetaT0(t0) 

'''
Devuelve el valor de la función beta_H(HR,e)
   que interviene en el cálculo de la función beta_c(t-t0)
   que define la evolución de la deformación en el tiempo.
   HR: Humedad relativa del aire expresada en tanto por uno.
   u: Perímetro de la pieza en contacto con la atmósfera expresado en metros.
   A: Área de la sección transversal de la pieza expresada en metros.
'''
def getBetaH(HR, u, A, fck):
    retval= 0.0
    tMp= 1500*(2*A/u)*(1+pow(1.2*HR,18))+250
    if((fck+8e6)<=35E6):
      retval= min(tMp,1500)
    else:
      alPha3= getAlpha3Fluencia(fck)
#      print 'alPha3=',alPha3
      retval= min(tMp,1500*alPha3)
    return retval

'''
Devuelve el valor de la función beta_c(t-t0,HR,e)
   que define la evolución de la deformación por fluencia en el tiempo.
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
   t: Instante en el que se calcula el valor del coeficiente
       expresado en días.
   t0: Instante a partir del cual actúa la carga que produce la fluencia
       expresado en días.
   HR: Humedad relativa del aire expresada en tanto por uno.
   u: Perímetro de la pieza en contacto con la atmósfera expresado en metros.
   A: Área de la sección transversal de la pieza expresada en metros.
'''
def getBetaC(fck, t, t0, HR, u, A):
#  print 'betaH=',getBetaH(HR,u,A,fck)
  return pow((t-t0)/(getBetaH(HR,u,A,fck)+(t-t0)),0.3)

'''
Devuelve el valor de la función phi(t,t0,HR,e)
   que define el coeficiente de fluencia según los comentarios al artículo 39.8.
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
   t: Instante en el que se calcula el valor del coeficiente
      expresado en días.
   t0: Instante a partir del cual actúa la carga que produce la fluencia
      expresado en días.
   HR: Humedad relativa del aire expresada en tanto por uno.
   u: Perímetro de la pieza en contacto con la atmósfera expresado en metros.
   A: Área de la sección transversal de la pieza expresada en metros.
'''
def getPhiFluencia(fck, t, t0, HR, u, A):
#  print 'Phi0=',getPhi0(fck,t0,HR,u,A)
#  print 'getBetaC=',getBetaC(fck,t,t0,HR,u,A)
  return getPhi0(fck,t0,HR,u,A)*getBetaC(fck,t,t0,HR,u,A)

'''
Devuelve el valor de la deformación por fluencia en el hormigón
   que se produce entre los instantes t0 y t.
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
   t0: Instante a partir del cual actúa la carga que produce la fluencia
      expresado en días.
   fiFluencia: Coeficiente de fluencia obtenido según los comentarios al artículo 39.8.
   s: Coeficiente que depende del tipo de cemento:
   Cementos de alta resistencia: s=0.2.
   Cementos normales: s= 0.25.
   Cementos de endurecimiento lento: s= 0.38
   sigmaT0: Tensión constante aplicada en t0.
  
'''
def getDeformacionFluencia(fck, t0, s, fiFluencia, sigmaT0):
  return sigmaT0*(1.0/auxEHE.getEcmT(fck,t0,s)+fiFluencia/auxEHE.getEcm(fck))

# ================= Retracción ==================
'''
Devuelve el valor del coeficiente de evolución temporal de la retracción
   por secado beta_ds(t,ts,e)
   que define la evolución de la deformación por fluencia en el tiempo.
   t: Instante en el que se calcula el valor del coeficiente
       expresado en días.
   tS: Instante a partir de que comienza la retracción (final del curado)
       expresado en días.
   u: Perímetro de la pieza en contacto con la atmósfera expresado en metros.
   A: Área de la sección transversal de la pieza expresada en metros.
'''
def getBetaDS(fck, t, tS, HR, u, A):
#  print 'betadstts=',(t-tS)/((t-tS)+0.04*pow(1000*(2*A/u),3/2))
  return (t-tS)/((t-tS)+0.04*pow(1000*(2*A/u),3/2))

'''
Puntos que definen la tabla 39.7.a de la EHE-08 para determinar el
   coeficiente de k_e que interviene en la expresión de la retracción
  por secado (EHE-08 comentarios al artículo 39.7)
'''
import scipy.interpolate
x= [0,.1,.2 ,.3 ,.5,1000]
y= [1, 1,.85,.75,.7,.7]
ptsKERetraccion=  scipy.interpolate.interp1d(x,y)

'''
Coeficiente de k_e que interviene en la expresión de la retracción
por secado (EHE-08 comentarios al artículo 39.7)
   u: Perímetro de la pieza en contacto con la atmósfera expresado en metros.
   A: Área de la sección transversal de la pieza expresada en metros.
'''
def getKERetraccion(u, A):
  return ptsKERetraccion(2*A/u)

'''
 Devuelve el valor del coeficiente de retracción a tiempo infinito
    (EHE-08 comentarios al artículo 39.7)
    que interviene en el cálculo de la retracción por secado.
    HR: Humedad relativa del aire expresada en tanto por uno.
'''
def getBetaHR(HR):
    retval= 0.0
    if(HR<0.99):
      retval= -1.55*(1-HR**3)
    else:
      retval= 0.25
#    print 'betaHR',retval
    return retval

'''
Coeficiente alpha_ds1 que introduce en la retracción
   por secado el efecto de la velocidad de endurecimiento
   del cemento.
'''
def getAlphaDS1(velCemento):
    if(velCemento=="lento"):
      return 3 
    elif(velCemento=="normal"):
      return 4 
    elif(velCemento=="rápido"):
      return 6 

'''
Coeficiente alpha_ds2 que introduce en la retracción
   por secado el efecto de la velocidad de endurecimiento
   del cemento.
'''
def getAlphaDS2(velCemento):
    if(velCemento=="lento"):
      return 0.13 
    elif (velCemento=="normal"):
      return 0.12 
    elif (velCemento=="rápido"):
      return 0.11

'''
Devuelve el coeficiente de retracción a tiempo infinito
   que interviene en el cálculo de la retracción
   por secado.
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
    HR: Humedad relativa del aire expresada en tanto por uno.
'''
def getEpsilonCDInf(fck, HR, velCemento):
    return 0.85e-6*getBetaHR(HR)*110*(2+getAlphaDS1(velCemento))*math.exp(-getAlphaDS2(velCemento)*(fck+8e6)/10e6)

'''
Devuelve la deformación por retracción por secado.
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
    HR: Humedad relativa del aire expresada en tanto por uno.
'''
def getEpsilonCD(fck, t, tS, HR, u, A, velCemento):
#    print 'Betadstts=',getBetaDS(fck,t,tS,HR,u,A)
#    print 'Kh=',getKERetraccion(u,A)
#    print 'Epscd0=',getEpsilonCDInf(fck,HR,velCemento)
    return getBetaDS(fck,t,tS,HR,u,A)*getKERetraccion(u,A)*getEpsilonCDInf(fck,HR,velCemento)

'''
Devuelve el valor del coeficiente de evolución temporal de la retracción
   autógena beta_as(t)
   que define la evolución de la deformación por fluencia en el tiempo.
   t: Instante en el que se calcula el valor del coeficiente
       expresado en días.
'''
def getBetaAS(t):
  return 1-math.exp(-0.2*math.sqrt(t))

'''
Devuelve el coeficiente de retracción a tiempo infinito
   que interviene en el cálculo de la retracción autógena.
   fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getEpsilonCAInf(fck):
  return -2.5e-6*(fck/1e6-10)

'''
Devuelve el la deformación por retracción autógena.
    t: Instante en el que se calcula el valor del coeficiente
       expresado en días.
  fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getEpsilonCA(fck, t):
  return getBetaAS(t)*getEpsilonCAInf(fck)

'''
Devuelve el la deformación por retracción.
    t: Instante en el que se calcula el valor del coeficiente
       expresado en días.
  fck: Resistencia característica del hormigón a 28 días expresada en Pa.
'''
def getDeformacionRetraccion(fck, t, tS, HR, u, A, velCemento):
  print 'ShrEpscd=',getEpsilonCD(fck,t,tS,HR,u,A,velCemento)
  print 'ShrEpsca=',getEpsilonCA(fck,t)
  return getEpsilonCD(fck,t,tS,HR,u,A,velCemento)+getEpsilonCA(fck,t)
