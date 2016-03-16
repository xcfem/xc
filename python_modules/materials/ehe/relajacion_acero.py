# -*- coding: utf-8 -*-

from __future__ import division
import math
import scipy.interpolate
# Relajación del acero según EHE-08 (Artº 38.9) y código modelo CEB-FIP 1990.

'''
Devuelve el coeficiente k para la expresión de la relajación
   a partir del tipo de acero. Ver Código Modelo 1990 apartado 2.3.4.5.
   tipoAcero: Tipo de acero (normal o superestabilizado).
'''
def getKRelajacion(tipoAcero):
  if(tipoAcero=="normal"):
    return 0.12 
  elif(tipoAcero=="superestabilizado"):
    return 0.19 
  else:
    lmsg.error("Tipo de acero: ",tipoAcero," desconocido.\n")
    return 0

'''
Puntos que definen la tabla 38.7.a de la EHE-08 para determinar la
   relajación a 1000 horas, en el caso de alambres y cordones, que intervienen
   en la expresión de la relajación para tiempos superiores a 1000 horas.
'''
x= [0,.5,.6,.7,.8]
y= [0,0,1,2,5.5]
ptsRO1000AlambresEHE= scipy.interpolate.interp1d(x,y)

'''
Puntos que definen la tabla 38.7.a de la EHE-08 para determinar la
   relajación a 1000 horas, en el caso de barras, que intervienen
   en la expresión de la relajación para tiempos superiores a 1000 horas.
'''
x= [0,.5,.6,.7,.8]
y= [0,0,2,3,7]
ptsRO1000BarrasEHE= scipy.interpolate.interp1d(x,y)

'''
Devuelve el valor de la relajación a 1000 horas
   Ver tabla 38.9.a en la página en EHE-08
   reinfSteelType: Tipo de acero (alambres, cordones o barras).
   alpha: Fracción de la tensión de rotura con la que se tesa el alambre.
'''
def getRO1000EHE(reinfSteelType, alpha):
  if(reinfSteelType=="alambre"):
    return ptsRO1000AlambresEHE(alpha) 
  elif(reinfSteelType=="cordon"):
    return ptsRO1000AlambresEHE(alpha)
  elif(reinfSteelType=="barra"):
    return ptsRO1000BarrasEHE(alpha)

'''
Puntos que definen la tabla 38.7.b de la EHE-08 para determinar la
   relajación para tiempos inferiores a a 1000 horas.
'''
x= []
y= []
x.append(0); y.append(0)
x.append(1); y.append(0.25)
x.append(5); y.append(0.45)
x.append(20); y.append(.55)
x.append(100); y.append(.7)
x.append(200); y.append(.8)
x.append(500); y.append(.9)
x.append(1000); y.append(1)
ptsRelajacionCortoPlazo= scipy.interpolate.interp1d(x,y)

'''
Devuelve el valor de la relajación para el instante t en función
   de la relajación obtenida para 1000 horas y del tipo de acero.
   reinfSteelType: Tipo de reinforcement (alambre, cordón o barra).
   tipoAcero: Tipo de acero (normal o superestabilizado).
   alpha: Fracción de la tensión de rotura con la que se tesa la reinforcement.
   t: Tiempo expresado en días (para facilitar el tratamiento conjunto con retraccion y
      fluencia).
  
'''
def getRelajacionTEHE(reinfSteelType, tipoAcero, alpha, tDias):
  tHoras= tDias*24
  RO1000= getRO1000EHE(reinfSteelType,alpha)
  if(tHoras<1000):
    return RO1000*ptsRelajacionCortoPlazo(tHoras)
  else:
    return RO1000*pow(tHoras/1000.0,getKRelajacion(tipoAcero))

'''
Devuelve el la pérdida de tensión por relajación del acero.
   reinfSteelType: Tipo de reinforcement (alambre, cordón o barra).
   tipoAcero: Tipo de acero (normal o estabilizado).
   alpha: Fracción de la tensión de rotura con la que se tesa la reinforcement.
   sigmaInic: Tensión inicial.
   t: Tiempo transcurrido expresado en días
      (para facilitar el tratamiento conjunto con retraccion y fluencia).
  
'''
def getPerdidaTensionRelajacionTEHE(reinfSteelType, tipoAcero, alpha, tDias, sigmaInicial):
  LGRO120= math.log10(getRelajacionTEHE(reinfSteelType,tipoAcero,alpha,120/24.0))
  LGRO1000= math.log10(getRelajacionTEHE(reinfSteelType,tipoAcero,alpha,1000/24.0))
  k2= (LGRO1000-LGRO120)/(3-math.log10(120))
  k1= LGRO1000-3*k2
  tHoras= tDias*24
  ROT= pow(10,k1+k2*math.log10(tHoras))/100.0
  return sigmaInicial*ROT

'''
Devuelve el la pérdida de tensión por relajación del acero
   a tiempo infinito según los comentarios al artículo 38.9.
   reinfSteelType: Tipo de reinforcement (alambre, cordón o barra).
   tipoAcero: Tipo de acero (normal o estabilizado).
   alpha: Fracción de la tensión de rotura con la que se tesa la reinforcement.
   sigmaInic: Tensión inicial.
  
'''
def getPerdidaTensionRelajacionFinalEHE(reinfSteelType, tipoAcero, alpha, sigmaInicial):
  ROFINAL= 2.9e-2*getRelajacionTEHE(reinfSteelType,tipoAcero,alpha,1000/24.0)
  return sigmaInicial*ROFINAL

'''
Puntos que definen la figura 2.3.3. del código modelo CEB-FIP 1990 para determinar la
   relajación a 1000 horas, en el caso de alambres y cordones de relajación
   normal (clase 1), que intervienen en la expresión de la relajación
   para tiempos superiores a 1000 horas.
'''
x= []
y= []
# Valores obtenidos del gráfico
x.append(0); y.append(0)
x.append(.5); y.append(0)
x.append(.6); y.append(4)
x.append(.7); y.append(12+1/9.0)
x.append(.8); y.append(12+1/9.0)
ptsRO1000Clase1CEB= scipy.interpolate.interp1d(x,y)

'''
Puntos que definen la figura 2.3.3. del código modelo CEB-FIP 1990 para determinar la
   relajación a 1000 horas, en el caso de alambres y cordones de baja relajación
   (clase 2), que intervienen en la expresión de la relajación
   para tiempos superiores a 1000 horas.
'''
x= []
y= []
# Valores obtenidos del gráfico
x.append(0); y.append(0)
x.append(.5); y.append(0)
x.append(.6); y.append(4/9.0)
x.append(.7); y.append(2)
x.append(.8); y.append(4+3.5/9.0)
ptsRO1000Clase2CEB= scipy.interpolate.interp1d(x,y)

'''
Puntos que definen la figura 2.3.3. del código modelo CEB-FIP 1990 para determinar la
   relajación a 1000 horas, en el caso de barras (clase 3),
   que intervienen en la expresión de la relajación para tiempos superiores a 1000 horas.
'''
x= []
y= []
# Valores obtenidos del gráfico
x.append(0); y.append(0)
x.append(.5); y.append(0)
x.append(.6); y.append(2)
x.append(.7); y.append(4)
x.append(.8); y.append(6+4/9.0)
ptsRO1000Clase3CEB= scipy.interpolate.interp1d(x,y)


'''
Devuelve el valor de la relajación a 1000 horas
   Ver figura 2.3.3. del código modelo CEB-FIP 1990
   reinfSteelType: Tipo de reinforcement (alambre, cordón o barra).
   tipoAcero: Tipo de acero (normal o estabilizado).
   alpha: Fracción de la tensión de rotura con la que se tesa el alambre.
'''
def getRO1000CEB(reinfSteelType, tipoAcero, alpha):
  if((reinfSteelType=="alambre") | (reinfSteelType=="cordon")):
    if(tipoAcero=="superestabilizado"):
      return ptsRO1000Clase2CEB(alpha)
    elif(tipoAcero=="normal"):
      return ptsRO1000Clase1CEB(alpha) 
  elif(reinfSteelType=="barra"):
    return ptsRO1000Clase3CEB(alpha)


'''
Devuelve el valor de la relajación para el instante t en función
   de la relajación obtenida para 1000 horas y del tipo de acero.
   reinfSteelType: Tipo de reinforcement (alambre, cordón o barra).
   tipoAcero: Tipo de acero (normal o estabilizado).
   alpha: Fracción de la tensión de rotura con la que se tesa la reinforcement.
   t: Tiempo expresado en días (para facilitar el tratamiento conjunto con retraccion y
      fluencia).
  
'''
def getRelajacionTCEB(reinfSteelType, tipoAcero, alpha, tDias):
  tHoras= tDias*24
  RO1000= getRO1000CEB(reinfSteelType,tipoAcero,alpha)
  if(tHoras<1000):
    return RO1000*ptsRelajacionCortoPlazo(tHoras)
  else:
    return RO1000*pow(tHoras/1000.0,getKRelajacion(tipoAcero))

'''
Devuelve el la pérdida de tensión por relajación del acero.
   reinfSteelType: Tipo de reinforcement (alambre, cordón o barra).
   tipoAcero: Tipo de acero (normal o estabilizado).
   alpha: Fracción de la tensión de rotura con la que se tesa la reinforcement.
   sigmaInic: Tensión inicial.
   t: Tiempo transcurrido expresado en días
      (para facilitar el tratamiento conjunto con retraccion y fluencia).
  
'''
def getPerdidaTensionRelajacionTCEB(reinfSteelType, tipoAcero, alpha, tDias, sigmaInicial):
  LGRO120= math.log10(getRelajacionTCEB(reinfSteelType,tipoAcero,alpha,120/24.0))
  LGRO1000= math.log10(getRelajacionTCEB(reinfSteelType,tipoAcero,alpha,1000/24.0))
  k2= (LGRO1000-LGRO120)/(3-math.log10(120))
  k1= LGRO1000-3*k2
  tHoras= tDias*24
  ROT= pow(10,k1+k2*math.log10(tHoras))/100.0
  return sigmaInicial*ROT
