# -*- coding: utf-8 -*-

from __future__ import division

import math
from miscUtils import LogMessages as lmsg
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
    return self.steelType.G()*self.It()
  def getWz(self,sectionClass= 1):
    if(sectionClass<3):
      return self.get('Wzpl')
    elif(sectionClass==3):
      return self.get('Wzel')
    else:
      lmsg.warning('cross sections of class: '+ str(sectionClass) + ' not implemented.')
  def getWy(self,sectionClass= 1):
    if(sectionClass<3):
      return self.get('Wypl')
    elif(sectionClass==3):
      return self.get('Wyel')
    else:
      lmsg.warning('cross sections of class: '+ str(sectionClass) + ' not implemented.')
  def getAeff(self,sectionClass= 1):
    '''Returns effective area depending of the cross-section class.'''
    if(sectionClass<3):
      return self.A()
    else:
      lmsg.warning('effective area for sections of class: '+ str(sectionClass) + ' not implemented.')
      
  def alphaY(self):
    return self.get('Avy')/self.A()
  def alphaZ(self):
    return self.get('Avz')/self.A()

  def setupULSControlVars(self,elems):
    '''For each element creates the variables
       needed to check ultimate limit state criterion to satisfy.'''
    for e in elems:
      e.setProp('FCTNCP',-1.0) #Normal stresses efficiency.
      e.setProp('N+',-6.023e23) #Positive axial force envelope
      e.setProp('N-',6.023e23) #Negative axial force envelope
      e.setProp('Mz+',-6.023e23) #Positive bending moment envelope
      e.setProp('Mz-',6.023e23) #Negative bending moment envelope
      e.setProp('My+',-6.023e23) #Positive bending moment envelope
      e.setProp('My-',6.023e23) #Negative bending moment envelope
      e.setProp('FCVCP',-1.0) #Shear stresses efficiency.
      e.setProp('Vy+',-6.023e23) #Positive y shear envelope
      e.setProp('Vy-',6.023e23) #Negative y shear  envelope

  def checkBiaxialBendingForElement(self,elem,nmbComb):
    '''Called in every commit to check biaxial bending criterion (bars in 3D problems).'''
    elem.getResistingForce()
    sectionClass= elem.getProp('sectionClass')
    chiLT= elem.getProp('chiLT')
    N= elem.getN
    My= (elem.getMy2-elem.getMy1)/2.0
    Mz= (elem.getMz2-elem.getMz1)/2.0
    Vy= elem.getVy
    FCTN= self.getBiaxialBendingEfficiency(sectionClass,N,My,Mz,Vy,chiLT)
    if(FCTN > elem.getProp("FCTNCP")):
      elem.setProp("FCTNCP",FCTN)
      elem.setProp("HIPCPTN",nmbComb)
    if(N>elem.getProp('N+')):
      elem.setProp("N+",N)
    if(N<elem.getProp('N-')):
      elem.setProp("N-",N)
    if(My>elem.getProp('My+')):
      elem.setProp("My+",My)
    if(My<elem.getProp('My-')):
      elem.setProp("My-",My)
    if(Mz>elem.getProp('Mz+')):
      elem.setProp("Mz+",Mz)
    if(Mz<elem.getProp('Mz-')):
      elem.setProp("Mz-",Mz)

  def checkYShearForElement(self,elem,nmbComb):
    '''Called in every commit to y shear criterion.'''
    elem.getResistingForce()
    sectionClass= elem.getProp('sectionClass')
    Vy= elem.getVy
    FCV= self.getYShearEfficiency(sectionClass,Vy)
    if(FCV > elem.getProp("FCVCP")):
      elem.setProp("FCVCP",FCV)
      elem.setProp("HIPCPV",nmbComb)
    if(Vy>elem.getProp('Vy+')):
      elem.setProp("Vy+",Vy)
    if(Vy<elem.getProp('Vy-')):
      elem.setProp("Vy-",Vy)
