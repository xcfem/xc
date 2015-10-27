# -*- coding: utf-8 -*-
'''
Funciones para comprobación de una sección a fisuración según el
artículo 49.2.4 de la EHE-08.
'''
from materials.fiber_section import creaSetsFibras
from materials.fiber_section import fiberUtils
from materials import crack_control_base as cc
import math

class CrackControlSIA262(cc.CrackControlBaseParameters):

  # Calcula la apertura característica de fisura.
  def calcRebarStress(self, scc):
    section= scc.getProp("datosSecc")
    tagHormigon= section.tipoHormigon.tagDiagK
    tagAcero= section.tipoArmadura.tagDiagK
    if(not scc.hasProp("rcSets")):
      scc.setProp("rcSets", creaSetsFibras.fiberSectionSetupRC3Sets(scc,tagHormigon,self.nmbSetFibrasHormigon,tagAcero,self.nmbSetFibrasArmadura))
    rcSets= scc.getProp("rcSets")
    fibrasHormigon= rcSets.fibrasHormigon.fSet
    fibrasArmadura= rcSets.fibrasArmadura.fSet
    armaduraTraccion= rcSets.tractionFibers

    self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
    self.numBarrasTracc= rcSets.getNumBarrasTraccion()
    if(self.numBarrasTracc>0):
      scc.calcRecubrimientos(self.nmbSetFibrasArmaduraTraccion)
      scc.calcSeparaciones(self.nmbSetFibrasArmaduraTraccion)
      self.eps1= fibrasHormigon.getStrainMax()
      self.eps2= max(fibrasHormigon.getStrainMin(),0.0)
      self.sepBarrasTracc= armaduraTraccion.getDistMediaFibras()
      self.areaBarrasTracc= armaduraTraccion.getArea(1)
      self.yCDGBarrasTracc= armaduraTraccion.getCdgY()
      self.zCDGBarrasTracc= armaduraTraccion.getCdgZ()
      self.tensMediaBarrasTracc= armaduraTraccion.getStressMed()
      self.iAreaMaxima=  fiberUtils.getIMaxPropFiber(armaduraTraccion,"getArea")
    return self.tensMediaBarrasTracc

def defVarsControlFISSIA262(elems):
  for e in elems:
    e.setProp("sg_sNeg",0)
    e.setProp("NCPNeg",0)
    e.setProp("MyCPNeg",0)
    e.setProp("MzCPNeg",0)
    e.setProp("HIPCPNeg","")
    e.setProp("sg_sPos",0)
    e.setProp("NCPPos",0)
    e.setProp("MyCPPos",0)
    e.setProp("MzCPPos",0)
    e.setProp("HIPCPPos","")

def trataResultsCombFISSIA262(preprocessor,nmbComb):
  # Comprobación de las secciones de hormigón frente a fisuración.
  print "Postproceso combinación: ",nmbComb,"\n"

  secHAParamsFis= CrackControlSIA262()
  elementos= preprocessor.getSets.getSet("total").getElements
  for e in elementos:
    scc= e.getSection()
    sigma_s= secHAParamsFis.calcRebarStress(scc)
    if(sigma_s>e.getProp("sg_sCP")):
      e.setProp("sg_sCP",sigma_s) # Caso pésimo
      e.setProp("HIPCP",nmbComb)
      Ntmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      MzTmp= scc.getStressResultantComponent("Mz")
      e.setProp("NCP",Ntmp)
      e.setProp("MyCP",MyTmp)
      e.setProp("MzCP",MzTmp)

def trataResultsCombFISSIA262PlanB(preprocessor,nmbComb):
  # Comprobación de las secciones de hormigón frente a fisuración estimando la tensión en la armadura.
  print "Postproceso combinación: ",nmbComb,"\n"

  secHAParamsFis= CrackControlSIA262()
  elementos= preprocessor.getSets.getSet("total").getElements
  for e in elementos:
    e.getResistingForce()
    scc= e.getSection()
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
    # eNC= datosScc.canto/3
    # exc= 0.0
    # As= max(datosScc.getAsPos(),datosScc.getAsNeg())
    # denom= 0.5*As*0.9*datosScc.canto
    # if(abs(Ntmp)<1e-6):
    #   sigma_s= MyTmp/denom
    # else:
    #   exc= abs(MyTmp/Ntmp)
    #   if(exc<eNC):
    #     sg= Ntmp/datosScc.getAc()
    #     sg+= MyTmp/datosScc.getI()*datosScc.canto/2
    #     sigma_s= 10*sg
    #   else:
    #     sigma_s= MyTmp/denom
    # print "eNC= ", eNC, " exc= ", exc, "sigma_s= ", sigma_s/1e6
    if(sigma_sPos>e.getProp("sg_sPos")):
      e.setProp("sg_sPos",sigma_sPos) # Caso pésimo
      e.setProp("HIPCPPos",nmbComb)
      e.setProp("NCPPos",Ntmp)
      e.setProp("MyCPPos",MyTmp)
      e.setProp("MzCPPos",MzTmp)
    if(sigma_sNeg>e.getProp("sg_sNeg")):
      e.setProp("sg_sNeg",sigma_sNeg) # Caso pésimo
      e.setProp("HIPCPNeg",nmbComb)
      e.setProp("NCPNeg",Ntmp)
      e.setProp("MyCPNeg",MyTmp)
      e.setProp("MzCPNeg",MzTmp)
