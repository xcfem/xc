# -*- coding: utf-8 -*-

from __future__ import division
import scipy.interpolate
import math

def getAlphaEcm(arido):
    '''getAlphaEcm(arido)
    Coeficiente corrector del módulo de deformación
    longitudinal del hormigón en función de la naturaleza
    del árido.
    '''
    if arido=="cuarcita":
        return 1
    elif arido=="arenisca":
        return 0.7
    elif arido=="caliza normal":
        return 0.9
    elif arido=="caliza densa":
        return 1.2
    elif arido=="volcanica porosa":
        return 0.9
    elif arido=="volcanica normal":
        return 1.2
    elif arido=="granito":
        return 1.1
    elif arido=="diabasa":
        return 1.3
    else:
        print "Error en tipo de árido."
        return 0.0


'''
Devuelve el coeficiente beta_cc definido en los
   comentarios del artículo 31.3 de la EHE-08, que
   representa la evolución en el tiempo de la resistencia
   del hormigón.
   t: Edad del hormigón expresada en días.
   s: Coeficiente que depende del tipo de cemento:
   Cementos de alta resistencia: s=0.2.
   Cementos normales: s= 0.25.
   Cementos de endurecimiento lento: s= 0.38  
'''
def getBetaCC(t,s):
  return math.exp(s*(1-math.sqrt(t/28)))



'''
Resistencia a compresión media del hormigón a una edad
   de t días estimada de acuerdo con los comentarios al artículo 31.3 de EHE-08.
  fck: Resistencia característica del hormigón a 28 días expresada en Pa.
   t: Edad del hormigón expresada en días.
   s: Coeficiente que depende del tipo de cemento:
   Cementos de alta resistencia: s=0.2.
   Cementos normales: s= 0.25.
   Cementos de endurecimiento lento: s= 0.38
  
'''
def getFcmT(fck,t,s):
    return (fck+8e6)*getBetaCC(t,s)


'''
Resistencia a compresión media del hormigón estimada de acuerdo con
los comentarios al artículo 39.6 de EHE-08.
  fck: Resistencia característica del hormigón a 28 días expresada en Pa.
  
'''
def getFcm(fck):
  return abs(fck)+8e6


'''
Modulo de deformación longitudinal secante a 28 días expresado
en Pa de acuerdo con el artículo 39.6 de EHE-08.
  fck: Resistencia característica del hormigón a 28 días expresada en Pa.
  arido: Naturaleza del árido empleado para elaborar el hormigón.
   
'''
def getEcm(fck):
  return 8500e6*(getFcm(fck)/1e6)**(1/3)
  
'''
Modulo de deformación longitudinal inicial a 28 días expresado
en Pa de acuerdo con el artículo 39.6 de EHE-08.
  fck: Resistencia característica del hormigón a 28 días expresada en Pa.
  arido: Naturaleza del árido empleado para elaborar el hormigón.
   
'''
def getEc0(fck,arido):
  return getAlphaEcm(arido)*getEcm(fck)


   
'''
Modulo de deformación longitudinal secante a t días expresado
en Pa de acuerdo con los comentarios al artículo 39.6 de EHE-08.
  fck: Resistencia característica del hormigón a 28 días expresada en Pa.
  t: Edad del hormigón expresada en días.
   s: Coeficiente que depende del tipo de cemento:
   Cementos de alta resistencia: s=0.2.
   Cementos normales: s= 0.25.
   Cementos de endurecimiento lento: s= 0.38
   
'''
def getEcmT(fck,t,s):
  return math.pow(getBetaCC(t,s),0.3)*getEcm(fck)
  

# Resistencia característica inferior del hormigón a tracción (según artículo 39.1 EHE)
def fctkInf(fck):
  return 0.21e6*(fck/1e6)**(2/3)
  
# Resistencia característica superior del hormigón a tracción (según artículo 39.1 EHE)
def fctkSup(fck):
  return 0.39e6*(fck/1e6)**(2/3)
  

# Resistencia media del hormigón a tracción (según artículo 39.1 EHE-08)
def fctMedEHE08(fck):
  if fck<50e6:
    return 0.30e6*(abs(fck/1e6))**(2/3)
  else:
    return 0.58e6*(abs(fck/1e6))**(1/2)
      
 
# Resistencia característica del hormigón a tracción (según artículo 39.1 EHE-08)
def fctkEHE08(fck):
    return 0.7*fctMedEHE08(fck)
  

'''
Factor a aplicar a la resistencia característica del hormigón a compresión en función de su edad en días.
   para hormigones de endurecimiento normal (tabla 30.4.b EHE)
'''
x= [-1.0,0.0,3.0,7.0,28.0,90,360,361]
y= [0.0,0.0,0.4,0.65,1.0,1.2,1.35,1.35]
factorRCompJDiasNormal= scipy.interpolate.interp1d(x,y)

'''
Factor a aplicar a la resistencia característica del hormigón a compresión en función de su edad en días.
   para hormigones de endurecimiento rápido (tabla 30.4.b EHE)
'''
x= [-1.0,0.0,3.0,7.0,28.0,90,360,361]
y= [0.0,0.0,0.55,0.75,1.0,1.15,1.20,1.20]
factorRCompJDiasRapido=  scipy.interpolate.interp1d(x,y)

'''
Factor a aplicar a la resistencia característica del hormigón a tracción en función de su edad en días.
   para hormigones de endurecimiento normal(tabla 30.4.c EHE)
'''
x= [-1.0,0.0,3.0,7.0,28.0,90,360,361]
y= [0.0,0.0,0.4,0.7,1.0,1.05,1.10,1.10]
factorRTraccJDiasNormal= scipy.interpolate.interp1d(x,y)

