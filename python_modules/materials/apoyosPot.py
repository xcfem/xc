# -*- coding: utf-8 -*-

import scipy.interpolate
from materials import typical_materials
import math

# Rigideces de apoyos de neopreno confinado

'''
Puntos que definen el coeficiente de rozamiento del teflón
   en función de la compresión media ver libro «Aparatos de
   apoyo en puentes» de AIPCR página 46
'''
# Tabla 59.8.2 del artículo 59.8.2 de EAE (página 256)
x= [5e6,10e6,20e6,30e6,45e6,1000e6]
y= [0.08,0.06,0.04,0.03,0.025,0.024]

tablaMuTeflon= scipy.interpolate.interp1d(x,y)

def getKhorizFicticiaTeflon(d):
  '''
  Rigidez ficticia frente al desplazamiento horizontal de
  un apoyo deslizante de teflón.
  Calculamos la rigidez de modo que cuando el desplazamiento
  sea de 20 mm la reacción del muelle sea igual a la fuerza
  de rozamiento.

  :param mu: Coeficiente de rozamiento Módulo elástico del elastómero.

  sg_media= 35 MPa tensión media en el teflón.

  :param d: Diámetro del apoyo.

  mov= 20e-3 Desplazamiento para el que se alcanza la fuerza de rozamiento.
  '''
  return tablaMuTeflon(35e6)*math.pi*(d/2.0)**2*35e6/20e-3

# Define material para modelizar el teflon.
def defineMaterialTeflon(preprocessor,d, matK):
  return typical_materials.defElasticMaterial(preprocessor, matK,getKhorizFicticiaTeflon(d))

# Define materiales para modelizar el teflon.
def defineMaterialesTeflon(preprocessor, d, matKX, matKY):
  defineMaterialTeflon(preprocessor,d,matKX)
  defineMaterialTeflon(preprocessor,d,matKY)
