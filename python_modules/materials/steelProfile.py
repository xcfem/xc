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
  def getAeff(self,sectionClass= 1):
    '''Returns effective area depending of the cross-section class.'''
    if(sectionClass<3):
      return self.A()
    else:
      logging.warning('effective area for sections of class: '+ str(sectionClass) + ' not implemented.')
      
  def alphaY(self):
    return self.get('Avy')/self.A()
  def alphaZ(self):
    return self.get('Avz')/self.A()

  def checkBiaxialBendingForElement(self,elem,nmbComb):
    '''Called in every commit to check biaxial bending criterion (bars in 3D problems).'''
    elem.getResistingForce()
    sectionClass= elem.getProp('sectionClass')
    chiLT= elem.getProp('chiLT')
    N1= elem.getN1
    My1= elem.getMy1
    Mz1= elem.getMz1
    Vy= elem.getVy
    FCTN= self.getBiaxialBendingEfficiency(sectionClass,N1,My1,Mz1,Vy,chiLT)
    if(FCTN > elem.getProp("FCTNCP")):
      elem.setProp("FCTNCP",FCTN)
      elem.setProp("NCP",N1)
      elem.setProp("MyCP",My1)
      elem.setProp("MzCP",Mz1)
      elem.setProp("VyCP",Vy)
      elem.setProp("HIPCPTN",nmbComb)

    N2= elem.getN2
    My2= elem.getMy2
    Mz2= elem.getMz2
    Vy= elem.getVy
    FCTN= self.getBiaxialBendingEfficiency(sectionClass,N2,My2,Mz2,Vy,chiLT)
    if(FCTN > elem.getProp("FCTNCP")):
      elem.setProp("FCTNCP",FCTN)
      elem.setProp("NCP",N2)
      elem.setProp("MyCP",My2)
      elem.setProp("MzCP",Mz2)
      elem.setProp("VyCP",Vy)
      elem.setProp("HIPCPTN",nmbComb)

  def checkYShearForElement(self,elem,nmbComb):
    '''Called in every commit to y shear criterion.'''
    elem.getResistingForce()
    sectionClass= elem.getProp('sectionClass')
    Vy= elem.getVy
    FCV= self.getYShearEfficiency(sectionClass,Vy)
    if(FCV > elem.getProp("FCVCP")):
      elem.setProp("FCVCP",FCV)
      elem.setProp("VyCP",Vy)
      elem.setProp("HIPCPV",nmbComb)
