# -*- coding: utf-8 -*-
'''
Funciones para comprobación de una sección a fisuración según el
artículo 49.2.4 de la EHE-08.
'''
import math
from materials.fiber_section import createFiberSets
from materials.fiber_section import fiberUtils
from materials import limit_state_checking_base as lscb
from postprocess import ControlVars as cv

class CrackControlSIA262(lscb.CrackControlBaseParameters):
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
    reinforcementTraccion= rcSets.tensionFibers

    self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
    self.numBarrasTracc= rcSets.getNumTensionRebars()
    if(self.numBarrasTracc>0):
      scc.calcRecubrimientos(self.tensionedRebarsFiberSetName)
      scc.calcSeparaciones(self.tensionedRebarsFiberSetName)
      self.eps1= concrFibers.getStrainMax()
      self.eps2= max(concrFibers.getStrainMin(),0.0)
      self.rebarsSpacingTracc= reinforcementTraccion.getAverageDistanceBetweenFibers()
      self.areaRebarTracc= reinforcementTraccion.getArea(1)
      self.yCDGBarrasTracc= reinforcementTraccion.getCdgY()
      self.zCDGBarrasTracc= reinforcementTraccion.getCdgZ()
      self.tensMediaBarrasTracc= reinforcementTraccion.getStressMed()
      self.iAreaMaxima=  fiberUtils.getIMaxPropFiber(reinforcementTraccion,"getArea")
    return self.tensMediaBarrasTracc

  def initControlVars(self,elements):
    '''
    Initialize control variables over elements.
    Parameters:
      elements:    elements to define control variables in
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
  # Comprobación de las secciones de hormigón frente a fisuración.
  print "Postprocesing combination: ",nmbComb,"\n"

  secHAParamsFis= CrackControlSIA262(limitStress)
  elements= preprocessor.getSets.getSet("total").getElements
  secHAParamsFis.check(elements,nmbComb)

def procesResultVerifFISSIA262PlanB(preprocessor,nmbComb,limitStress):
  # Comprobación de las secciones de hormigón frente a fisuración estimando la tensión en la reinforcement.
  print "Postprocesing combination: ",nmbComb,"\n"

  secHAParamsFis= CrackControlSIA262PlanB(limitStress)
  elements= preprocessor.getSets.getSet("total").getElements
  secHAParamsFis.check(elements,nmbComb)
