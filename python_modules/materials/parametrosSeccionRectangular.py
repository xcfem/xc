# -*- coding: utf-8 -*-
from __future__ import division
import scipy.interpolate
import sys
from materials import typical_materials
from materials import sectionProperties as sp

# Puntos que definen el valor de alpha en función de h/b
#   ver libro hormigón Jiménez Montoya 14a. edición página 405
x= [1,1.25,1.5,2,3,4,6,10,10000]
y= [0.14,0.171,0.196,0.229,0.263,0.281,0.299,0.313,1.0/3.0]
tablaAlphaTorsion= scipy.interpolate.interp1d(x,y)


# Devuelve el valor del coeficiente alpha en función de h/b
#   ver libro hormigón Jiménez Montoya 14a. edición página 405
def getAlphaTorsion(b,h):
  retval= 0.0
  if b<h:
    retval= tablaAlphaTorsion(h/b)
  else:
    retval= tablaAlphaTorsion(b/h)
  return retval

# Puntos que definen el valor de beta en función de h/b
#   ver libro hormigón Jiménez Montoya 14a. edición página 405
x= [1,1.25,1.5,2,3,4,6,8,10,10000]
y= [0.208, 0.221, 0.231, 0.246, 0.267, 0.282, 0.299, 0.307, 0.313, 1.0/3]
tablaBetaTorsion=  scipy.interpolate.interp1d(x,y)

# Devuelve el valor del coeficiente beta en función de h/b
#   ver libro hormigón Jiménez Montoya 14a. edición página 405}
def getBetaTorsion( b, h):
  retval= 0.0
  if b<h:
    retval= tablaBetaTorsion(h/b)
  else:
    retval= tablaBetaTorsion(b/h)
  return retval

# Devuelve el momento de inercia a torsión de una sección rectangular.
#   Ver libro hormigón Jiménez Montoya 14a. edición página 405
def getJTorsion( b, h):
  alphaJT= getAlphaTorsion(b,h)
  retval= 0.0
  if b<h:
    retval= alphaJT*pow(b,3)*h
  else:
    retval= alphaJT*b*pow(b,3)
  return retval

class RectangularSection(sp.sectionProperties):
  """Rectangular section geometric parameters
  Attributes:
    name:         name identifying the section
    b:            cross-section width (parallel to local z axis)
    h:            cross-section height (parallel to local y axis)
    E:            Young’s modulus of the material
    nu:           Poisson’s ratio
  """
  def __init__(self,name,b,h,E,nu):
    super(RectangularSection,self).__init__(name,E,nu)
    self.b= b
    self.h= h
  def A(self):
    """ Section's area."""
    return self.b*self.h
  def Iy(self):
    return 1/12.0*self.h*self.b**3
  def Iz(self):
    return 1/12.0*self.b*self.h**3
  def J(self):
    return getJTorsion(self.b,self.h)
  def Wyel(self):
    return self.Iy()/(self.b/2.0)
  def Wzel(self):
    return self.Iz()/(self.h/2.0)
  def alphaY(self):
    return 5.0/6.0 #Coeficiente de distorsión, ver libro E. Oñate pág. 122.
  def alphaZ(self):
    return self.alphaY()

