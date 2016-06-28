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

  '''
  def __init__(self,name,area,Iy,Iz,Jtors,Wy,Wz):
    self.nmb= name
    self.area=area
    self.Iy=Iy
    self.Iz=Iz
    self.Jtors=Jtors
    self.Wy=Wy
    self.Wz=Wz
  def A(self):
    '''cross-sectional area'''
    return self.area
  def Iy(self):
    '''second moment of area about the local y-axis'''
    raise "Abstract method, please override"
    return self.Iy
  def Iz(self):
    '''second moment of area about the local z-axis'''
    return self.Iz
  def J(self):
    '''torsional moment of inertia of the section'''
    return self.Jtors
  def Wyel(self):
    '''section modulus with respect to local y-axis'''
    return self.Wy
  def Wzel(self):
    '''section modulus with respect to local z-axis'''
    return self.Wz

