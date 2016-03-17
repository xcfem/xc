# -*- coding: utf-8 -*-

from __future__ import division

import math
from miscUtils import LogMessages as lmsg
from materials import typical_materials
from materials import sectionProperties as sp
from materials.ec3 import lateral_torsional_buckling as ltb
from postprocess import def_vars_control as vc


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
    vc.defVarsEnvelopeInternalForcesBeamElems(elems)
    for e in elems:
      e.setProp('FCTNCP',[-1.0,-1.0]) #Normal stresses efficiency.
      e.setProp('FCVCP',[-1.0,-1.0]) #Shear stresses efficiency.

  def checkUniaxialBendingForElement(self,elem,nmbComb):
    '''Called in every commit to check uniaxial bending criterion (bars in 2D problems).'''
    elem.getResistingForce()
    sectionClass= elem.getProp('sectionClass')
    chiLT= elem.getProp('chiLT')
    N1= elem.getN1
    M1= -elem.getM1
    V1= elem.getV1
    FCTN1= self.getZBendingEfficiency(sectionClass,M1,V1,chiLT)
    N2= elem.getN2
    M2= elem.getM2
    V2= -elem.getV2
    FCTN2= self.getZBendingEfficiency(sectionClass,M2,V2,chiLT)

    fctn= elem.getProp("FCTNCP")
    if(FCTN1 > fctn[0]):
      fctn[0]= FCTN1
      elem.setProp("HIPCPTN1",nmbComb)
    if(FCTN2 > fctn[1]):
      fctn[1]= FCTN2
      elem.setProp("HIPCPTN2",nmbComb)
    elem.setProp("FCTNCP",fctn)

    vc.updateEnvelopeInternalForcesBeamElem2D(elem)

  def checkBiaxialBendingForElement(self,elem,nmbComb):
    '''Called in every commit to check biaxial bending criterion (bars in 3D problems).'''
    elem.getResistingForce()
    sectionClass= elem.getProp('sectionClass')
    chiLT= elem.getProp('chiLT')
    N1= elem.getN1
    My1= elem.getMy1
    Mz1= -elem.getMz1
    Vy1= elem.getVy1
    FCTN1= self.getBiaxialBendingEfficiency(sectionClass,N1,My1,Mz1,Vy1,chiLT)
    N2= elem.getN2
    My2= -elem.getMy2
    Mz2= elem.getMz2
    Vy2= -elem.getVy2
    FCTN2= self.getBiaxialBendingEfficiency(sectionClass,N2,My2,Mz2,Vy2,chiLT)

    fctn= elem.getProp("FCTNCP")
    if(FCTN1 > fctn[0]):
      fctn[0]= FCTN1
      elem.setProp("HIPCPTN1",nmbComb)
    if(FCTN2 > fctn[1]):
      fctn[1]= FCTN2
      elem.setProp("HIPCPTN2",nmbComb)
    elem.setProp("FCTNCP",fctn)

    vc.updateEnvelopeInternalForcesBeamElem(elem)

  def checkYShearForElement(self,elem,nmbComb):
    '''Called in every commit to y shear criterion.'''
    elem.getResistingForce()
    sectionClass= elem.getProp('sectionClass')
    Vy1= elem.getVy1
    FCV1= self.getYShearEfficiency(sectionClass,Vy1)
    Vy2= elem.getVy2
    FCV2= self.getYShearEfficiency(sectionClass,Vy2)

    fcv= elem.getProp("FCVCP")
    if(FCV1 > fcv[0]):
      fcv[0]= FCV1
      elem.setProp("HIPCPV1",nmbComb)
    if(FCV2 > fcv[1]):
      fcv[1]= FCV2
      elem.setProp("HIPCPV2",nmbComb)
    elem.setProp("FCVCP",fcv)
