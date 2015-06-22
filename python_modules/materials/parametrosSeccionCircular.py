# -*- coding: utf-8 -*-

import math
from materials import typical_materials
from materials import sectionProperties as sp

class CircularSection(sp.sectionProperties):
  """Circular section parameters"""
  r= 0.0 # radius.
  def __init__(self,name,r,E,nu):
    super(CircularSection,self).__init__(name,E,nu)
    self.r= r
  def A(self):
    """ Section's area."""
    return math.pi*self.r*self.r
  def Iy(self):
    return 1.0/4.0*math.pi*self.r**4
  def Iz(self):
    return self.Iy()
  def J(self):
    return 2*self.Iy()
  def alphaY(self):
    return 6.0/7.0 #Coeficiente de distorsión, ver libro E. Oñate pág. 122.
  def alphaZ(self):
    return self.alphaY()


# Define las propiedades "derivadas" de una sección circular.
def setupSeccCircular(nmbSeccion,r,E,nu):
  return CircularSection(nmbSeccion,r,E,nu)

# Define una sección elástica para elementos 3d a partir de los datos del registro.
def defSeccCircularElastica3d(mdlr, defSecc):
  print "DEPRECATED; call defSeccElastica3d."
  return defSeccElastica3d(mdlr,defSecc)


# Define una sección elástica para elementos 3d a partir de los datos del registro.
def defSeccCircularShElastica3d(mdlr, defSecc):
  print "DEPRECATED; call defSeccShElastica3d."
  return defSeccShElastica3d(mdlr,defSecc)


# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccCircularElastica2d(mdlr, defSecc):
  print "DEPRECATED; call defSeccElastica2d."
  return defSeccElastica2d(mdlr,defSecc)

# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccCircularShElastica2d(mdlr, defSecc):
  print "DEPRECATED; call defSeccShElastica2d."
  return defSeccShElastica2d(mdlr,defSecc)

