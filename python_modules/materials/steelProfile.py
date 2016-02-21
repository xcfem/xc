# -*- coding: utf-8 -*-

import math
from materials import typical_materials
from materials import sectionProperties as sp
from materials.ec3 import lateral_torsional_buckling as ltb

class SteelProfile(sp.sectionProperties):
  """Structural steel section properties"""
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
    ''' Torsional constant.'''
    return self.get('It')
  def It(self):
    ''' Torsional constant.'''
    return self.get('It')
  def Iw(self):
    ''' Warping constant.'''
    return self.get('Iw')
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
  def getMcr(self,xi,Mi,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0):
    '''Returns elastic critical moment about minor axis: y
       xi: abcissae for the moment diagram
       Mi: ordinate for the moment diagram
       ky: lateral bending coefficient ky= 1.0 => free lateral bending
                                       ky= 0.5 => prevented lateral bending
       kw: warping coefficient kw= 1.0 => free warping
                               ky= 0.5 => prevented warping
       k1: warping AND lateral bending coefficient at left end
                               k1= 1.0 => free warping AND lateral bending
                               k1= 0.5 => prevented warp. AND lateral bending
       k2: warping AND lateral bending coefficient at right end
                               k2= 1.0 => free warping AND lateral bending
                               k2= 0.5 => prevented warp. AND lateral bending'''
    mgf= ltb.MomentGradientFactorC1(xi,Mi)
    L= mgf.getL()
    C1= mgf.getC1(k1,k2)
    pi2EIy= math.pi**2*self.getEIy()
    GJ= self.getGJ()
    kyL2= (ky*L)**2
    return C1*pi2EIy/kyL2*math.sqrt((self.ky/self.kw)**2*self.Iw()/self.Iy()+kyl2*GJ/pi2EIy)
