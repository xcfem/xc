# -*- coding: utf-8 -*-
from rough_calculations import ng_simple_bending_reinforcement
import math
import xc_base
import geom

from postprocess import control_vars as cv
from materials import limit_state_checking_base as lsc

def VuNoShearRebars(concrete,steel,Nd,Md,AsTrac,b,d):
  '''Section shear capacity without shear reinforcement.

     :param concrete: concrete material.
     :param steel: steel material of the tensioned reinforcement.
     :param Nd: axial internal force on the section.
     :param Md: bending moment on the section.
     :param AsTrac: area of tensioned reinforcement.
     :param b: section width.
     :param d: section effective depth.
  '''
  Mu= ng_simple_bending_reinforcement.Mu(AsTrac,concrete.fcd(),steel.fyd(),b,d) 
  return VuNoShearRebarsSIA262(concrete,Nd,Md,Mu,b,d)

def VuNoShearRebarsSIA262(concrete,Nd,Md,Mu,b,d):
  '''Section shear capacity without shear reinforcement. 
     Simplified method according to document: «Dalles sans étriers soumises
     à l'effort tranchant» par Aurelio Muttoni (file: beton-VD1V2.pdf).'''
  h=d/0.9   #mejorar
  if Nd<=0:
    mDd=-Nd*(h/2-d/3)
  else:
    mDd=-Nd*(h/2-(h-d))
  kv= 2.2*(Md-mDd)/(Mu-mDd)
  kd= 1/(1+kv*d)
  taucd= concrete.taucd()
  return kd*taucd*b*d

def VuShearRebars90SIA262(Asw,s,steel,z,alpha=math.radians(30)):
  '''Shear capacity of shear reinforcement.'''  
  cot_alpha= 1.0/math.tan(alpha)
  retval= Asw/s*z*steel.fyd()*cot_alpha
  return retval

def VuWithShearRebarsSIA262(concrete,steel,Nd,Md,Mu,b,d,Asw,s,z,alpha=math.radians(30)):
  '''Section shear capacity with shear reinforcement.'''  
  Vcu= VuNoShearRebarsSIA262(concrete,Nd,Md,Mu,b,d)
  Vsu= VuShearRebars90SIA262(Asw,s,steel,z,alpha)
  return Vcu+Vsu

class ShearController(lsc.LimitStateControllerBase):
  '''Object that controls shear limit state with SIA 262.'''

  def __init__(self,limitStateLabel):
    super(ShearController,self).__init__(limitStateLabel)

  def setSection(self,rcSection):
    self.concrete= rcSection.concrType #Arreglar
    self.steel= rcSection.reinfSteelType
    self.width= rcSection.b
    self.depthUtil= 0.9* rcSection.h
    self.mechanicLeverArm= 0.9*self.depthUtil #Mejorar
    self.AsTrsv= rcSection.shReinfZ.getAs()
    self.s= rcSection.shReinfZ.shReinfSpacing
    self.Vcu= 0.0 # Concrete contribution to the shear strength.
    self.Vsu= 0.0 # Rebar contribution to the shear strength.

  def calcVcu(self, Nd, Md, Mu):
    ''' Computes the shear strength of the section without shear reinforcement.'''
    self.Vcu= VuNoShearRebarsSIA262(self.concrete,Nd,abs(Md),abs(Mu),self.width,self.depthUtil)
   
  def calcVsu(self):
    ''' Computes the shear strength of the section without shear reinforcement.
        s= 1.0 because AsTrsv ya incorpora todas las ramas en un metro.'''
    self.Vsu= VuShearRebars90SIA262(self.AsTrsv,1.0,self.steel,self.mechanicLeverArm)

  def calcVu(self, Nd, Md, Mu, Vd):
    '''  Calcula el cortante último de la sección.'''
    self.Vcu= 0.0
    self.Vsu= 0.0
    self.calcVcu(Nd,abs(Md),abs(Mu))
    self.calcVsu()
    return self.Vu()

  def Vu(self):
    return self.Vcu+self.Vsu

  def initControlVars(self,elements):
    '''
    Initialize control variables over elements.
    Parameters:
      elements:    elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.RCShearControlVars())

  def check(self,elements,nmbComb):
    '''
    Comprobación de las secciones de hormigón frente a cortante.
       XXX Falta tener en cuenta la dirección de las barras de refuerzo
       a cortante.
    '''
    print "Postprocesing combination: ",nmbComb
    # XXX Ignoramos la deformación por torsión.

    for e in elements:
      e.getResistingForce()
      scc= e.getSection()
      idSection= e.getProp("idSection")
      section= scc.getProp("datosSecc")
      self.setSection(section)
      #concreteCode= section.concrType
      #codArmadura= section.reinfSteelType
      AsTrsv= section.shReinfY.getAs()
      alpha= section.shReinfY.angAlphaShReinf
      theta= section.shReinfY.angThetaConcrStruts

      VuTmp= section.getRoughVcuEstimation() 
      NTmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      momentThreshold= VuTmp/1000.0
      if(abs(MyTmp)<momentThreshold): #Too small moment.
        MyTmp= momentThreshold
      MzTmp= scc.getStressResultantComponent("Mz")
      if(abs(MzTmp)<momentThreshold): #Too small moment.
        MzTmp= momentThreshold
      VyTmp= scc.getStressResultantComponent("Vy")
      if(abs(VyTmp)>VuTmp/5.0): #We "eliminate" very small shear forces.
        posEsf= geom.Pos3d(NTmp,MyTmp,MzTmp)
        diagInt= e.getProp("diagInt")
        intersection= diagInt.getIntersection(posEsf)

        Mu= intersection.z
        VuTmp= self.calcVu(NTmp,MzTmp, Mu, VyTmp) #Mz associated with Vy
      if(VuTmp!=0.0):
        FCtmp= abs(VyTmp)/VuTmp
      else:
        FCtmp= 10
      if(FCtmp>=e.getProp(self.limitStateLabel).CF):
        VzTmp= scc.getStressResultantComponent("Vz")
        e.setProp(self.limitStateLabel,cv.RCShearControlVars(idSection,nmbComb,FCtmp,NTmp,MyTmp,MzTmp,Mu,VyTmp,VzTmp,theta,self.Vcu,self.Vsu,VuTmp)) # Worst case

