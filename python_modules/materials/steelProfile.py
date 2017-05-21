# -*- coding: utf-8 -*-

from __future__ import division

import math
from miscUtils import LogMessages as lmsg
from materials import typical_materials
from materials import section_properties as sp
from postprocess import def_vars_control as vc

#Alpha imperfection factor.
def alphaImperfectionFactor(bucklingCurve):
  '''Returns the alpha imperfection factor called defined
     in tables 6.1 and 6.2 of EC3.

     :param bucklingCurve: buckling curve (a0,a,b,c or d).
  '''
  retval= 0.76
  if(bucklingCurve=='a0'):
    retval= 0.13
  elif(bucklingCurve=='a'):
    retval= 0.21
  elif(bucklingCurve=='b'):
    retval= 0.34
  elif(bucklingCurve=='c'):
    retval= 0.49
  elif(bucklingCurve=='d'):
    retval= 0.76
  else:
    lmsg.error('Buckling curve: '+str(bucklingCurve)+' unknown.')
  return retval


class SteelProfile(sp.SectionProperties):
  '''Properties of a section in structural steel

  :ivar steel:    steel object (e.g. S275JR)
  :ivar table:    module containing a dictionary with mechanical characteristics
                  of a series of profiles 
                  (e.g. materials.perfiles_metalicos.arcelor.perfiles_he_arcelor)
  :ivar name:     name identifying the section in the table
  '''
  def __init__(self,steel,name,table):
    self.steelType= steel
    self.profil= table[name]
#    super(SteelProfile,self).__init__(name,self.profil['E'],self.profil['nu'])
    super(SteelProfile,self).__init__(name)
  def get(self,code):
    return self.profil[code]
  def A(self):
    ''':returns: cross-sectional area of the section'''
    return self.get('A')
  def Iy(self):
    ''':returns: second moment of area about the local y-axis'''
    return self.get('Iy')
  def getGyrationRadiusY(self):
    ''':returns: radius of gyration about the local y-axis'''
    return math.sqrt(self.Iy()/self.A())
  def getSlendernessY(self,Leq):
    '''
       :param Leq: buckling length for the column.
       :returns: buckling slendernes relative to the local y-axis
    '''
    return Leq/self.getGyrationRadiusY()
  def getAdimensionalSlendernessY(self,Leq,sectionClass= 1):
    '''
       :param Leq: buckling length for the column.
       :param sectionClass: class of the section (1, 2, 3 or 4).
       :returns: adimensional slendernes as defined in EC3 part 1 5.5.1.2
    '''
    betaA= self.getAeff(sectionClass)/self.A()
    lambdA= self.getSlendernessY(Leq)
    lambda1= self.steelType.getLambda1()
    return lambdA/lambda1*math.sqrt(betaA)
  def getBucklingReductionFactorY(self,Leq,bucklingCurve,sectionClass= 1):
    '''
       :param Leq: buckling length for the column.
       :param bucklingCurve: buckling curve (a0,a,b,c or d).
       :param sectionClass: class of the section (1, 2, 3 or 4).
       :returns: bucling reduction factor as defined in EC3-1-1 6.3.1
    '''
    alpha= alphaImperfectionFactor(bucklingCurve)
    lmb= self.getAdimensionalSlendernessY(Leq,sectionClass= 1)
    phi= 0.5*(1+alpha*(lmb-0.2)+lmb**2)
    return min(1.0/(phi+math.sqrt(phi**2-lmb**2)),1.0)
  def getBucklingResistanceY(self,Leq,bucklingCurve,sectionClass= 1):
    '''
       :param Leq: buckling length for the column.
       :param bucklingCurve: buckling curve (a0,a,b,c or d).
       :param sectionClass: class of the section (1, 2, 3 or 4).
       :returns: bucling reduction factor as defined in EC3-1-1 6.3.1
    '''
    X= self.getBucklingReductionFactorY(Leq,bucklingCurve,sectionClass)
    return X*self.getAeff(sectionClass)*self.steelType.fyd()
  def Iz(self):
    ''':returns: second moment of area about the local z-axis'''
    return self.get('Iz')
  def getGyrationRadiusZ(self):
    ''':returns: radius of gyration about the local z-axis'''
    return math.sqrt(self.Iz()/self.A())
  def getSlendernessZ(self,Leq):
    '''
       :param Leq: buckling length for the column.
       :returns: buckling slendernes relative to the local z-axis
    '''
    return Leq/self.getGyrationRadiusZ()
  def getAdimensionalSlendernessZ(self,Leq,sectionClass= 1):
    '''
       :param Leq: buckling length for the column.
       :param sectionClass: class of the section (1, 2, 3 or 4).
       :returns: adimensional slendernes as defined in EC3 part 1 5.5.1.2
    '''
    betaA= self.getAeff(sectionClass)/self.A()
    lambdA= self.getSlendernessZ(Leq)
    lambda1= self.steelType.getLambda1()
    return lambdA/lambda1*math.sqrt(betaA)
  def getBucklingReductionFactorZ(self,Leq,bucklingCurve,sectionClass= 1):
    '''
       :param Leq: buckling length for the column.
       :param bucklingCurve: buckling curve (a0,a,b,c or d).
       :param sectionClass: class of the section (1, 2, 3 or 4).
       :returns: bucling reduction factor as defined in EC3-1-1 6.3.1
    '''
    alpha= alphaImperfectionFactor(bucklingCurve)
    lmb= self.getAdimensionalSlendernessZ(Leq,sectionClass= 1)
    phi= 0.5*(1+alpha*(lmb-0.2)+lmb**2)
    return min(1.0/(phi+math.sqrt(phi**2-lmb**2)),1.0)
  def getBucklingResistanceZ(self,Leq,bucklingCurve,sectionClass= 1):
    '''
       :param Leq: buckling length for the column.
       :param bucklingCurve: buckling curve (a0,a,b,c or d).
       :param sectionClass: class of the section (1, 2, 3 or 4).
       :returns: bucling reduction factor as defined in EC3-1-1 6.3.1
    '''
    X= self.getBucklingReductionFactorZ(Leq,bucklingCurve,sectionClass)
    return X*self.getAeff(sectionClass)*self.steelType.fyd()
  def getBucklingResistance(self,Leq,bucklingCurveY,bucklingCurveZ,sectionClass= 1):
    '''
       :param Leq: buckling length for the column.
       :param bucklingCurve: buckling curve (a0,a,b,c or d).
       :param sectionClass: class of the section (1, 2, 3 or 4).
       :returns: bucling reduction factor as defined in EC3-1-1 6.3.1
    '''
    rY= self.getBucklingResistanceY(Leq,bucklingCurveY,sectionClass)
    rZ= self.getBucklingResistanceZ(Leq,bucklingCurveZ,sectionClass)
    return min(rY,rZ)
  def J(self):
    ''':returns: torsional moment of inertia of the section'''
    return self.get('It')
  def It(self):
    ''':returns: torsional constant.'''
    return self.get('It')
  def Iw(self):
    ''':returns: warping constant.'''
    return self.get('Iw')
  def EIy(self):
    return self.steelType.E*self.Iy()
  def EIz(self):
    return self.steelType.E*self.Iz()
  def GJ(self):
    return self.steelType.G()*self.It()
  def getWz(self,sectionClass= 1):
    ''':returns: section modulus with respect to local z-axis'''
    if(sectionClass<3):
      return self.get('Wzpl')
    elif(sectionClass==3):
      return self.get('Wzel')
    else:
      lmsg.warning('cross sections of class: '+ str(sectionClass) + ' not implemented.')
  def getWy(self,sectionClass= 1):
    ''':returns: section modulus with respect to local y-axis'''
    if(sectionClass<3):
      return self.get('Wypl')
    elif(sectionClass==3):
      return self.get('Wyel')
    else:
      lmsg.warning('cross sections of class: '+ str(sectionClass) + ' not implemented.')
  def getAeff(self,sectionClass= 1):
    ''':returns: effective area depending of the cross-section class.'''
    retval= self.A()
    if(sectionClass>=3):
      lmsg.warning('effective area for sections of class: '+ str(sectionClass) + ' not implemented.')
      retval/=100.0
    return retval
  def alphaY(self):
    ''':returns: shear shape factor with respect to local y-axis'''
    return self.get('Avy')/self.A()
  def alphaZ(self):
    ''':returns: shear shape factor with respect to local z-axis'''
    return self.get('Avz')/self.A()
  def getNcrY(self,Leq):
    '''
     Theoretical critical axial force on y axis.

     :param Leq: buckling length for the column.
     :returns: theoretical critical axial force on y axis. 
    '''
    return math.pi**2*self.EIy()/(Leq**2)
  def getNcrZ(self,Leq):
    '''
     Theoretical critical axial force on z axis.

     :param Leq: buckling length for the column.
     :returns: theoretical critical axial force on z axis. 
    '''
    return math.pi**2*self.EIz()/(Leq**2)
  def getNcr(self,Leq):
    '''
     Theoretical critical axial force (minimum of NcrY and NcrZ).

     :param Leq: buckling length for the column.
     :returns: theoretical critical axial force  (minimum of NcrY and NcrZ). 
    '''
    return min(self.getNcrY(Leq),self.getNcrZ(Leq))

  
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
    M1= elem.getM1
    V1= elem.getV1
    FCTN1= self.getZBendingEfficiency(sectionClass,M1,V1,chiLT)
    N2= elem.getN2
    M2= elem.getM2
    V2= elem.getV2
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
    Mz1= elem.getMz1
    Vy1= elem.getVy1
    FCTN1= self.getBiaxialBendingEfficiency(sectionClass,N1,My1,Mz1,Vy1,chiLT)
    N2= elem.getN2
    My2= elem.getMy2
    Mz2= elem.getMz2
    Vy2= elem.getVy2
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
