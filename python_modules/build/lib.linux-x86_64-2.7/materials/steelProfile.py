# -*- coding: utf-8 -*-

import math
from materials import typical_materials
from materials import sectionProperties as sp

class SteelProfile(sp.sectionProperties):
  """Structural steel section properties"""
  profil= None # record in table.
  steelType= None
  def __init__(self,steel,name,table):
    self.steelType= steel
    self.profil= table[name]
    super(SteelProfile,self).__init__(name,self.profil['E'],self.profil['nu'])
  def get(self,code):
    return self.profil[code]
  def A(self):
    """ Section's area."""
    return self.get('A')
  def Iy(self):
    return self.get('Iy')
  def Iz(self):
    return self.get('Iz')
  def J(self):
    return self.get('It')
  def EIy(self):
    return self.steelType.E*self.Iy()
  def EIz(self):
    return self.steelType.E*self.Iz()
  def GJ(self):
    return self.steelType.E*self.It()
  def alphaY(self):
    return self.get('Avy')/self.A()
  def alphaZ(self):
    return self.get('Avz')/self.A()

