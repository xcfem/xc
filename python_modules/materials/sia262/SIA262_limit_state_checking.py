# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import scipy.interpolate
from materials.sia262 import SIA262_materials
import geom
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from rough_calculations import ng_simple_bending_reinforcement
import math
import xc_base
import geom
from materials.sections.fiber_section import createFiberSets
from materials.sections.fiber_section import fiberUtils
from materials.sections import stressCalc as sc

# Returns adherence stress (Pa) for concrete type (tableau 19 SIA 262).
adherenceStress_x= [12e6 , 16e6,20e6 ,25e6 ,30e6 ,35e6 ,40e6 ,45e6 ,50e6]
adherenceStress_y= [1.5e6,1.8e6,2.1e6,2.4e6,2.7e6,3.0e6,3.3e6,3.6e6,3.8e6]
adherenceStress= scipy.interpolate.interp1d(adherenceStress_x,adherenceStress_y)

def getBasicAnchorageLength(phi,fck, fsd):
  """Returns anchorage length according to SIA 262."""
  fbd= adherenceStress(abs(fck))
  return max(fsd/fbd/4.0,25.0)*phi
     
# SIA 262 section 4.4.2

def AsMinContrainteLimiteeTraction(concrete,sgAdm,t):
  fctm= concrete.fctm()
  kt= SIA262_materials.reductionFactorKT(t)
  return kt*fctm*t/sgAdm

def AsMinTraction(concrete,exigence,ecartement,t):
  fctm= concrete.fctm()
  kt= SIA262_materials.reductionFactorKT(t)
  sgAdm= SIA262_materials.limitationContraintes(exigence,ecartement)
  return kt*fctm*t/sgAdm

def AsMinFlexion(concrete,concreteCover,exigence,ecartement,t):
  fctd= concrete.fctm()*SIA262_materials.reductionFactorKT(t/3)
  z= 0.9*(t-concreteCover)
  w= 1/6.0*t**2
  sgAdm= SIA262_materials.limitationContraintes(exigence,ecartement)
  return fctd*w/sgAdm/z

#Check normal stresses limit state.

class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase):
  '''Object that controls normal stresses limit state.'''

  def __init__(self,limitStateLabel):
    super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)

  def initControlVars(self,elements):
    '''Initialize control variables over elements.

      :param elements: elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.BiaxialBendingControlVars())

  def check(self,elements, nmbComb):
    '''Launch checking.
    
      :param elements: elements to check
    '''
    #print "Postprocessing combination: ",nmbComb
    for e in elements:
      e.getResistingForce()
      TagTmp= e.tag
      scc= e.getSection()
      idSection= e.getProp("idSection")
      Ntmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      MzTmp= scc.getStressResultantComponent("Mz")
      posEsf= geom.Pos3d(Ntmp,MyTmp,MzTmp)
      diagInt= e.getProp("diagInt")
      CFtmp= diagInt.getCapacityFactor(posEsf)
      if(CFtmp>e.getProp(self.limitStateLabel).CF):
        e.setProp(self.limitStateLabel,cv.BiaxialBendingControlVars(idSection,nmbComb,CFtmp,Ntmp,MyTmp,MzTmp)) # Worst case.

class UniaxialBendingNormalStressController(lsc.LimitStateControllerBase):
  '''Object that controls normal stresses limit state (uniaxial bending).'''

  def __init__(self,limitStateLabel):
    super(UniaxialBendingNormalStressController,self).__init__(limitStateLabel)

  def initControlVars(self,elements):
    '''Initialize control variables over elements.

      :param elements: elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.BiaxialBendingControlVars())

  def check(self,elements, nmbComb):
    '''
    Parameters:
      elements:    elements to check
    '''
    #print "Postprocessing combination: ",nmbComb
    for e in elements:
      e.getResistingForce()
      TagTmp= e.tag
      scc= e.getSection()
      idSection= e.getProp("idSection")
      Ntmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      posEsf= geom.Pos2d(Ntmp,MyTmp)
      diagInt= e.getProp("diagInt")
      CFtmp= diagInt.getCapacityFactor(posEsf)
      if(CFtmp>e.getProp(self.limitStateLabel).CF):
        e.setProp(self.limitStateLabel,cv.BiaxialBendingControlVars(idSection,nmbComb,CFtmp,Ntmp,MyTmp)) # Worst case.

# Shear checking.

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
    '''  Computes section ultimate shear strength.'''
    self.Vcu= 0.0
    self.Vsu= 0.0
    self.calcVcu(Nd,abs(Md),abs(Mu))
    self.calcVsu()
    return self.Vu()

  def Vu(self):
    return self.Vcu+self.Vsu

  def initControlVars(self,elements):
    ''' Initialize control variables over elements.

      :param elements: elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.RCShearControlVars())

  def check(self,elements,nmbComb):
    '''
    Check the shear strength of the RC section.
       XXX Orientation of the transverse reinforcement is not
       taken into account.
    '''
    print "Postprocesing combination: ",nmbComb
    # XXX torsional deformation ingnored.

    for e in elements:
      e.getResistingForce()
      scc= e.getSection()
      idSection= e.getProp("idSection")
      section= scc.getProp("datosSecc")
      self.setSection(section)
      AsTrsv= section.shReinfY.getAs()
      alpha= section.shReinfY.angAlphaShReinf
      theta= section.shReinfY.angThetaConcrStruts

      VuTmp= section.getRoughVcuEstimation() 
      NTmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      momentThreshold= VuTmp/1000.0
      if(abs(MyTmp)<momentThreshold): #bending moment too small.
        MyTmp= momentThreshold
      MzTmp= scc.getStressResultantComponent("Mz")
      if(abs(MzTmp)<momentThreshold): #bending moment too small.
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


class CrackControlSIA262(lsc.CrackControlBaseParameters):
  '''Crack control checking of a reinforced concrete section
   according to SIA 262.'''
  def __init__(self,limitStateLabel,limitStress):
    super(CrackControlSIA262,self).__init__(limitStateLabel)
    self.limitStress= limitStress #Limit value for rebar stresses.
  def calcRebarStress(self, scc):
    '''Returns average stress in rebars.'''
    section= scc.getProp("datosSecc")
    concreteTag= section.concrType.matTagK
    reinfMatTag= section.reinfSteelType.matTagK
    if(not scc.hasProp("rcSets")):
      scc.setProp("rcSets", createFiberSets.fiberSectionSetupRC3Sets(scc,concreteTag,self.concreteFibersSetName,reinfMatTag,self.rebarFibersSetName))
    rcSets= scc.getProp("rcSets")
    concrFibers= rcSets.concrFibers.fSet
    reinfFibers= rcSets.reinfFibers.fSet
    tensionedReinforcement= rcSets.tensionFibers

    self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
    self.tensionedRebars.number= rcSets.getNumTensionRebars()
    if(self.tensionedRebars.number>0):
      scc.computeCovers(self.tensionedRebarsFiberSetName)
      scc.computeSpacement(self.tensionedRebarsFiberSetName)
      self.eps1= concrFibers.getStrainMax()
      self.eps2= max(concrFibers.getStrainMin(),0.0)
      self.tensionedRebars.setup(tensionedReinforcement)
    return self.tensionedRebars.averageStress

  def initControlVars(self,elements):
    ''' Initialize control variables over elements.

      :param elements: elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.CrackControlVars(idSection= e.getProp('idSection')))

  def check(self,elements,nmbComb):
    '''Crack control checking.'''
    print "REWRITE NEEDED. See equivalent function in  CrackControlSIA262PlanB."
    for e in elements:
      scc= e.getSection()
      idSection= e.getProp("idSection")
      sigma_s= self.calcRebarStress(scc)
      if(sigma_s>e.getProp("sg_sCP")):
        e.setProp("sg_sCP",sigma_s) # Caso pésimo
        e.setProp("HIPCP",nmbComb)
        Ntmp= scc.getStressResultantComponent("N")
        MyTmp= scc.getStressResultantComponent("My")
        MzTmp= scc.getStressResultantComponent("Mz")
        e.setProp("NCP",Ntmp)
        e.setProp("MyCP",MyTmp)
        e.setProp("MzCP",MzTmp)


class CrackControlSIA262PlanB(CrackControlSIA262):
  def __init__(self,limitStateLabel,limitStress):
    super(CrackControlSIA262PlanB,self).__init__(limitStateLabel,limitStress)
  def check(self,elements,nmbComb):
    '''Crack control.'''
    for e in elements:
      e.getResistingForce()
      scc= e.getSection()
      idSection= e.getProp("idSection")
      Ntmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      MzTmp= scc.getStressResultantComponent("Mz")
      datosScc= scc.getProp("datosSecc")
      stressCalc= datosScc.getStressCalculator()
      stressCalc.solve(Ntmp, MyTmp)
      sigma_sPos= stressCalc.sgs
      sigma_sNeg= stressCalc.sgsp
      sigma_c= stressCalc.sgc
      #print "sgc0= ", stressCalc.sgc0
      # sigma_s= 0.0
      # eNC= datosScc.depth/3
      # exc= 0.0
      # As= max(datosScc.getAsPos(),datosScc.getAsNeg())
      # denom= 0.5*As*0.9*datosScc.depth
      # if(abs(Ntmp)<1e-6):
      #   sigma_s= MyTmp/denom
      # else:
      #   exc= abs(MyTmp/Ntmp)
      #   if(exc<eNC):
      #     sg= Ntmp/datosScc.getAc()
      #     sg+= MyTmp/datosScc.getI()*datosScc.depth/2
      #     sigma_s= 10*sg
      #   else:
      #     sigma_s= MyTmp/denom
      # print "eNC= ", eNC, " exc= ", exc, "sigma_s= ", sigma_s/1e6
      CFPos= sigma_sPos/self.limitStress #Positive face capacity factor.
      CFNeg= sigma_sNeg/self.limitStress #Negative face capacity factor.
      elementControlVars= None
      if(e.hasProp(self.limitStateLabel)):
        elementControlVars= e.getProp(self.limitStateLabel)
      else:
        elementControlVars= cv.CrackControlVars(idSection,cv.CrackControlBaseVars(nmbComb,CFPos,Ntmp,MyTmp,MzTmp,sigma_sPos),CrackControlBaseVars(nmbComb,CFNeg,Ntmp,MyTmp,MzTmp,sigma_sNeg))
      if(CFPos>elementControlVars.crackControlVarsPos.CF):
        elementControlVars.crackControlVarsPos= cv.CrackControlBaseVars(nmbComb,CFPos,Ntmp,MyTmp,MzTmp,sigma_sPos)
      if(CFNeg>elementControlVars.crackControlVarsNeg.CF):
        elementControlVars.crackControlVarsNeg= cv.CrackControlBaseVars(nmbComb,CFNeg,Ntmp,MyTmp,MzTmp,sigma_sNeg)
      e.setProp(self.limitStateLabel,elementControlVars)


def procesResultVerifFISSIA262(preprocessor,nmbComb,limitStress):
  '''Crack control checking of reinforced concrete sections.'''
  print "Postprocesing combination: ",nmbComb,"\n"

  secHAParamsFis= CrackControlSIA262(limitStress)
  elements= preprocessor.getSets.getSet("total").getElements
  secHAParamsFis.check(elements,nmbComb)

def procesResultVerifFISSIA262PlanB(preprocessor,nmbComb,limitStress):
  '''Crack control checking of reinforced concrete sections.'''
  print "Postprocesing combination: ",nmbComb,"\n"

  secHAParamsFis= CrackControlSIA262PlanB(limitStress)
  elements= preprocessor.getSets.getSet("total").getElements
  secHAParamsFis.check(elements,nmbComb)

# Control of fatigue limit state according to SIA 262.

def estimateSteelStress(sccData, N, M, As, y):
  retval= 0.0
  eNC= sccData.depth/3.0
  exc= 0.0
  denom= math.copysign(0.8*As*0.9*sccData.depth,y)
  retval= M/denom
  if(retval<0.0):
    f= M/(0.8*0.9*sccData.depth)
    retval= -f/(0.2**sccData.getAc())*10.0
  if(abs(N)>1e-6):
    exc= abs(M/N)
    if(exc<eNC):
      retval= N/sccData.getAc()*10.0
      retval+= M/sccData.getI()*y
  return retval

def estimateSteelStressPos(sccData, N, M):
  retval= 0.0
  eNC= sccData.depth/3.0
  exc= 0.0
  As= sccData.getAsPos()
  y = sccData.getYAsPos()
  return estimateSteelStress(sccData, N, M, As, y)

def estimateSteelStressNeg(sccData, N, M):
  retval= 0.0
  As= sccData.getAsNeg()
  y = sccData.getYAsNeg()
  return estimateSteelStress(sccData, N, M, As, y)

def estimateSigmaC(sccData, sigma_sPos, sigma_sNeg):
  sgMax= max(sigma_sPos,sigma_sNeg)
  retval= 0.0
  if(sgMax<0.0):
    retval= (sigma_sPos+sigma_sNeg)/2.0/10.0
  else:
    sgMin= min(sigma_sPos,sigma_sNeg)
    if(sgMin<=0):
      Ac= 0.1*sccData.getAc()
      if(sgMax==sigma_sPos):
        retval= -sgMax*sccData.getAsPos()/Ac
      else:
        retval= -sgMax*sccData.getAsNeg()/Ac
  return retval

def estimateSigmaCPlanB(sccData, N, M):
  Ac= 0.1*sccData.getAc()
  Ic= sccData.getI()
  sg1= N/Ac+M/Ic*sccData.depth/2.0
  sg2= N/Ac-M/Ic*sccData.depth/2.0
  sgc= min(min(sg1,sg2),0.0)
  return 2.0*sgc #Ver Jiménez Montoya 12.3 (page 244)

def getConcreteLimitStress(sccData,kc,controlVars):
  '''4.3.8.3.1 SIA 262 2013'''
  fcd= sccData.concrType.fcd()
  concreteStresses= controlVars.getConcreteMaxMinStresses()
  sg_max= concreteStresses[0]
  sg_min= concreteStresses[1]
  return min(-0.5*kc*fcd+0.45*abs(sg_min),-0.9*kc*fcd)

def getShearLimit(sccData,controlVars,vu):
  '''4.3.8.3.2 SIA 262 2013'''
  v_0= controlVars.state0.Vy
  v_1= controlVars.state1.Vy
  vd_min= min(v_0,v_1)
  vd_max= max(v_0,v_1)
  coc= vd_min
  if(abs(vd_max)<1e-12):
    if(vd_max<0):
      coc*=-1 #change sign.
  else:
    coc/= vd_max
  if(coc>=0):
    retval= min(0.5*vu+0.45*abs(vd_min),0.9*vu)
  else:
    retval= 0.5*vu-abs(vd_min)
  if(retval<0):
    print "limite negativo = ", retval
  return retval

def getShearCF(controlVars):
  '''Fatigue shear capacity factor.'''
  return max(abs(controlVars.state0.Vy),abs(controlVars.state1.Vy))/controlVars.shearLimit

class FatigueController(lsc.LimitStateControllerBase):
  '''Object that controls RC fatigue limit state.'''

  def __init__(self,limitStateLabel):
    super(FatigueController,self).__init__(limitStateLabel)

  def initControlVars(self,elements):
    ''' Defines limit state control variables for all the elements.'''
    for e in elements:
      idS= e.getProp("idSection")
      e.setProp(self.limitStateLabel,cv.FatigueControlVars(idSection= idS))

  def check(self,elements, combNm):
    '''
    Checks the fatigue limit state for all the elements in the given set and
    for the  combination passed as a parameter. The verification is carried out
    following the procedure proposed in the standard SIA262 (art. 4.3.8)

    :param elements:  xc set of elements to be checked
    :param combNm:    combination name e.g.:

        - ELUF0: unloaded structure (permanent loads)
        - ELUF1: fatigue load in position 1.
        - ELUF2: fatigue load in position 2 (XXX not yet implemented!!!).
        - ELUF3: fatigue load in position 3 (XXX not yet implemented!!!).
        - ...
    '''
    print 'Controlling limit state: ',self.limitStateLabel, ' for load combination: ',combNm,"\n"


    index= int(combNm[-1])
    for e in elements:
      e.getResistingForce()
      scc= e.getSection()
      N= scc.getStressResultantComponent("N")
      My= scc.getStressResultantComponent("My")
      Mz= scc.getStressResultantComponent("Mz")
      Vy= scc.getStressResultantComponent("Vy")
      Vz= scc.getStressResultantComponent("Vz")
      datosScc= scc.getProp("datosSecc")
      stressCalc= datosScc.getStressCalculator()
      stressCalc.solve(N, My)
      sigma_sPos= stressCalc.sgs
      sigma_sNeg= stressCalc.sgsp
      sigma_c= stressCalc.sgc
      #print "sgc0= ", stressCalc.sgc0
      controlVars= e.getProp(self.limitStateLabel)
      resultsComb= cv.FatigueControlBaseVars(combNm,-1.0,N, My, Mz,Vy,sigma_sPos, sigma_sNeg,sigma_c)
      if(index==0):
        controlVars.state0= resultsComb
      else:
        controlVars.state1= resultsComb
        kc= 1.0 #XXX  SIA 262 4.2.1.7
        controlVars.concreteLimitStress= getConcreteLimitStress(datosScc,kc,controlVars)
        #print "concreteLimitStress= ", controlVars.concreteLimitStress/1e6
        controlVars.concreteBendingCF= controlVars.getConcreteMinStress()/controlVars.concreteLimitStress
        
        #print "concreteBendingCF= ",controlVars.concreteBendingCF
        section= scc.getProp("datosSecc")
        secHAParamsCortante= ShearController(self.limitStateLabel)
        secHAParamsCortante.setSection(section)
        posEsf= geom.Pos3d(N,My,Mz)
        diagInt= e.getProp("diagInt")
        FCflex= diagInt.getCapacityFactor(posEsf)
        controlVars.Mu= My/FCflex
        controlVars.Vu= secHAParamsCortante.calcVu(N,My, controlVars.Mu, Vy)
        
        controlVars.shearLimit= getShearLimit(section,controlVars,controlVars.Vu)
        controlVars.concreteShearCF= getShearCF(controlVars)
        e.setProp(self.limitStateLabel,controlVars)
