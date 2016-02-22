# -*- coding: utf-8 -*-

from __future__ import division

import math
import logging
from materials import typical_materials
from materials import sectionProperties as sp
from materials.ec3 import lateral_torsional_buckling as ltb

logging.addLevelName( logging.WARNING, "\033[1;31m%s\033[1;0m" % logging.getLevelName(logging.WARNING))
logging.addLevelName( logging.ERROR, "\033[1;41m%s\033[1;0m" % logging.getLevelName(logging.ERROR))

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
    return self.steelType.G()*self.It()
  def getWz(self,sectionClass= 1):
    if(sectionClass<3):
      return self.get('Wzpl')
    elif(sectionClass==3):
      return self.get('Wzel')
    else:
      logging.warning('cross sections of class: '+ str(sectionClass) + ' not implemented.')
  def getWy(self,sectionClass= 1):
    if(sectionClass<3):
      return self.get('Wypl')
    elif(sectionClass==3):
      return self.get('Wyel')
    else:
      logging.warning('cross sections of class: '+ str(sectionClass) + ' not implemented.') 
      
  def alphaY(self):
    return self.get('Avy')/self.A()
  def alphaZ(self):
    return self.get('Avz')/self.A()
  def getMcr(self,xi,Mi,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0):
    '''Returns elastic critical moment about minor axis: y
       Calculation is made following the paper:

       A. López, D. J. Yong, M. A. Serna,
       Lateral-torsional buckling of steel beams: a general expression for
       the moment gradient factor.
       (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

       Parameters:
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
    pi2EIy= math.pi**2*self.EIy()
    GIt= self.GJ()
    kyL2= (ky*L)**2
    Mcr0= pi2EIy/kyL2
    sum1= (ky/kw)**2*self.Iw()/self.Iy()
    sum2= GIt/Mcr0
    f2= math.sqrt(sum1+sum2)
    # print '  L= ', L
    # print '  kyL2= ', kyL2
    # print '  GJ= ', GIt/1e3
    # print '  Iw= ', self.Iw()*100**6, ' cm^6'
    # print '  C1= ', C1
    # print '  Mcr0=', Mcr0/1e3  
    # print '  sum1= ', sum1
    # print '  sum2= ', sum2
    # print '  f2= ', f2
    return C1*Mcr0*f2

  def getNonDimensionalBeamSlenderness(self,sectionClass,xi,Mi,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0):
    '''Returns non dimensional beam slenderness
       see parameter definition on method getMcr.'''
    Mcr= self.getMcr(xi,Mi,ky,kw, k1, k2)
    return math.sqrt(self.getWz(sectionClass)*self.steelType.fy/Mcr)
