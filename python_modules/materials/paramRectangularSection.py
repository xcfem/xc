# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "

import scipy.interpolate
import sys
from materials import typical_materials
from materials import sectionProperties as sp

# Puntos que definen el valor de alpha en función de h/b
#   ver libro hormigón Jiménez Montoya 14a. edición página 405
x= [1,1.25,1.5,2,3,4,6,10,10000]
y= [0.14,0.171,0.196,0.229,0.263,0.281,0.299,0.313,1.0/3.0]
tablaAlphaTorsion= scipy.interpolate.interp1d(x,y)


def getAlphaTorsion(b,h):
  ''':returns: alpha coefficient of a rectangular sections, as a function of  h/b
  Reference: concrete book Jiménez Montoya 14a. edition page 405

  :param b:            cross-section width (parallel to local z-axis)
  :param h:            cross-section height (parallel to local y-axis)
  '''
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

def getBetaTorsion( b, h):
  ''':returns: beta coefficient of a rectangular sections, as a function of  h/b
  Reference: concrete book Jiménez Montoya 14a. edition page 405

  :param b:            cross-section width (parallel to local z-axis)
  :param h:            cross-section height (parallel to local y-axis)
  '''
  retval= 0.0
  if b<h:
    retval= tablaBetaTorsion(h/b)
  else:
    retval= tablaBetaTorsion(b/h)
  return retval

def getJTorsion( b, h):
  ''':returns: torsional moment of inertia of a rectangular section
  Reference: concrete book Jiménez Montoya 14a. edition page 405

  :param b:            cross-section width (parallel to local z-axis)
  :param h:            cross-section height (parallel to local y-axis)
  '''
  alphaJT= getAlphaTorsion(b,h)
  retval= 0.0
  if b<h:
    retval= alphaJT*pow(b,3)*h
  else:
    retval= alphaJT*b*pow(b,3)
  return retval

class RectangularSection(sp.sectionProperties):
  '''Rectangular section geometric parameters
  
  :ivar name:         name identifying the section
  :ivar b:            cross-section width (parallel to local z-axis)
  :ivar h:            cross-section height (parallel to local y-axis)
  '''
  def __init__(self,name,b,h):
    super(RectangularSection,self).__init__(name)
    self.b= b
    self.h= h
  def A(self):
    ''':returns: cross-sectional area of the section'''
    return self.b*self.h
  def Iy(self):
    ''':returns: second moment of area about the local y-axis'''
    return 1/12.0*self.h*self.b**3
  def Iz(self):
    ''':returns: second moment of area about the local z-axis'''
    return 1/12.0*self.b*self.h**3
  def J(self):
    ''':returns: torsional moment of inertia of the section'''
    return getJTorsion(self.b,self.h)
  def Wyel(self):
    ''':returns: section modulus with respect to local y-axis'''
    return self.Iy()/(self.b/2.0)
  def Wzel(self):
    ''':returns: section modulus with respect to local z-axis'''
    return self.Iz()/(self.h/2.0)
  def alphaY(self):
    ''':returns: shear shape factor with respect to local y-axis'''
    return 5.0/6.0 #Coeficiente de distorsión, ver libro E. Oñate pág. 122.
  def alphaZ(self):
    ''':returns: shear shape factor with respect to local z-axis'''
    return self.alphaY()

