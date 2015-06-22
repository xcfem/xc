# -*- coding: utf-8 -*-
from __future__ import division
# Macros para facilitar el cálculo de la acción del viento según el CTE}

import math
def ParamK(gae):
    '''
    Parámetro k en función del grado de aspereza del entorno
    según CTE (tabla D.2).
    '''
    if gae==1:
          return 0.15
    elif gae==2:
          return 0.17
    elif gae==3:
          return 0.19
    elif gae==4:
          return 0.22
    elif gae==5:
          return 0.24
    else:
        print "Error en el grado de aspereza del entorno."
        return 0.0

def ParamL(gae):
    '''
    Parámetro L en función del grado de aspereza del entorno
    según CTE (tabla D.2).}
    '''
    if gae==1:
          return 0.003
    elif gae==2:
          return 0.01
    elif gae==3:
          return 0.05
    elif gae==4:
          return 0.3
    elif gae==5:
          return 1.0
    else:
        print "Error en el grado de aspereza del entorno."
        return 0.0
      
def ParamZ(gae):
    '''
    Parámetro Z en función del grado de aspereza del entorno
    según CTE (tabla D.2).
    '''
    if gae==1:
          return 1
    elif gae==2:
          return 1
    elif gae==3:
          return 2
    elif gae==4:
          return 5
    elif gae==5:
          return 10
    else:
      
        print "Error en el grado de aspereza del entorno."
        return 0.0
      
def ParamF(gae,z):
    '''
    Parámetro F en función del grado de aspereza del entorno
    y de la cota del punto según CTE (expresión D.3).
    '''
    k= ParamK(gae)
    L= ParamL(gae)
    Z= ParamZ(gae)
    if z<Z:
        ZM= Z
    else:
        ZM= z
    return k*math.log(ZM/L)
  

def CoefExp(gae,z):
    '''
    Coeficiente de exposición en función del grado de aspereza del entorno
    y de la cota del punto sobre el terreno según CTE (expresión D.2).
    '''
    if z>200:
        print "Expresión no válida para cotas sobre el terreno superiores a 200 m."
    k= ParamK(gae)
    F= ParamF(gae,z)
    return F*(F+7*k)

def interpolaCoefsPresion(A,Cpe1,Cpe10):
    '''
    Función para interpolar coeficientes de presión 
    según la expresión D.4 del CTE (SE-AE-27).
    '''
    return Cpe1+(Cpe10-Cpe1)*math.log10(A)
  
