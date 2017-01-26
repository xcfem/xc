# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "


import math
from materials import typical_materials
from materials import sectionProperties as sp

class CircularSection(sp.sectionProperties):
  '''Geometric parameters of a circular or circular hollow section

  :ivar name:      name identifying the section
  :ivar Rext:      external radius
  :ivar Rint:      internal radius
   '''
  r= 0.0 # radius.
  def __init__(self,name,Rext):
    super(CircularSection,self).__init__(name)
    self.Rext= Rext
  def A(self):
    ''':returns: cross-sectional area of the section'''
    return math.pi*self.Rext*self.Rext
  def Iy(self):
    ''':returns: second moment of area about the local y-axis'''
    return 1.0/4.0*math.pi*self.Rext**4
  def Iz(self):
    ''':returns: second moment of area about the local z-axis'''
    return self.Iy()
  def J(self):
    ''':returns: torsional moment of inertia of the section'''
    return 2*self.Iy()
  def alphaY(self):
    ''':returns: distorsion coefficient with respect to local Y axis
     '''
    return 6.0/7.0 #Coeficiente de distorsión, ver libro E. Oñate pág. 122.
  def alphaZ(self):
    ''':returns: distorsion coefficient with respect to local Z axis'''
    return self.alphaY()


# Define las propiedades "derivadas" de una sección circular.
def setupSeccCircular(sectionName,r):
  return CircularSection(sectionName,r)

# Define una sección elástica para elementos 3d a partir de los datos del registro.
def defSeccCircularElastica3d(preprocessor, defSecc):
  print "DEPRECATED; call defSeccElastica3d."
  return defSeccElastica3d(preprocessor,defSecc)


# Define una sección elástica para elementos 3d a partir de los datos del registro.
def defSeccCircularShElastica3d(preprocessor, defSecc):
  print "DEPRECATED; call defSeccShElastica3d."
  return defSeccShElastica3d(preprocessor,defSecc)


# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccCircularElastica2d(preprocessor, defSecc):
  print "DEPRECATED; call defSeccElastica2d."
  return defSeccElastica2d(preprocessor,defSecc)

# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccCircularShElastica2d(preprocessor, defSecc):
  print "DEPRECATED; call defSeccShElastica2d."
  return defSeccShElastica2d(preprocessor,defSecc)

