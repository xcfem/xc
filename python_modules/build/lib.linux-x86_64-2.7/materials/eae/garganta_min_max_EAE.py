# -*- coding: utf-8 -*-
'''
Devuelve el espesor de garganta mínimo para un cordón en ángulo que une una chapa de espesor t
   según el artículo 59.3.2 de la EAE (página 296)
    t: Espesor de la chapa.
  
'''
def aMinAngulo(t):
  if(t<=10e-3):
    return 3e-3
  else:
    if(t<=20e-3):
      return 4.5e-3
    else:
      return 5.6e-3

'''
Devuelve el espesor de garganta máximo para un cordón en ángulo que une una chapa de espesor t
   según el artículo 59.3.2 de la EAE (página 296)
    t: Espesor de la chapa.
  
'''
def aMaxAngulo(t):
  return 0.7*t

'''
Devuelve el espesor de garganta mínimo para unir dos chapas
 según el artículo 59.3.2 de la EAE (página 296)
  t1: Espesor de la chapa 1.
  t2: Espesor de la chapa 2.
  
'''
def aMinAnguloChapas(t1, t2):
  amin1= aMinAngulo(t1)
  amin2= aMinAngulo(t2)
  return max(amin1,amin2)

'''
Devuelve el espesor de garganta máximo para unir dos chapas
   según el artículo 59.3.2 de la EAE (página 296)
    t1: Espesor de la chapa 1.
    t2: Espesor de la chapa 2.
  
'''
def aMaxAnguloChapas(t1, t2):
  amax1= aMaxAngulo(t1)
  amax2= aMaxAngulo(t2)
  return min(amax1,amax2)
