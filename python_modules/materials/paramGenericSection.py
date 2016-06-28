# -*- coding: utf-8 -*-
from __future__ import division
import sys
from materials import typical_materials
from materials import sectionProperties as sp


class GenericSection(sp.sectionProperties):
  '''Mechanical properties of generic section 

  :ivar name:         name identifying the section
  :ivar area:         cross-sectional area
  :ivar Iy:           second moment of area about the local y-axis
  :ivar Iz:           second moment of area about the local z-axis
  :ivar Jtors:        torsional moment of inertia of the section
  :ivar Wy:           section modulus with respect to local y-axis
  :ivar Wz:           section modulus with respect to local z-axis
  :ivar alphY:        shear shape factor with respect to local y-axis
  :ivar alphZ:        shear shape factor with respect to local z-axis
  '''
  def __init__(self,name,area,I_y,I_z,Jtors,W_y,W_z,alphY,alphZ):
    self.nmb= name
    self.area=area
    self.I_y=I_y
    self.I_z=I_z
    self.Jtors=Jtors
    self.W_y=W_y
    self.W_z=W_z
    self.alphY=alphY
    self.alphZ=alphZ
  def A(self):
    ''':returns: cross-sectional area'''
    return self.area
  def Iy(self):
    ''':returns: second moment of area about the local y-axis'''
    return self.I_y
  def Iz(self):
    ''':returns: second moment of area about the local z-axis'''
    return self.I_z
  def J(self):
    ''':returns: torsional moment of inertia of the section'''
    return self.Jtors
  def Wyel(self):
    ''':returns: section modulus with respect to local y-axis'''
    return self.W_y
  def Wzel(self):
    ''':returns: section modulus with respect to local z-axis'''
    return self.W_z
  def alphaY(self):
    ''':returns: shear shape factor with respect to local y-axis'''
    return self.alphY #Coeficiente de distorsión, ver libro E. Oñate pág. 122.
  def alphaZ(self):
    ''':returns: shear shape factor with respect to local z-axis'''
    return self.alphZ

