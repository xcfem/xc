# -*- coding: utf-8 -*-
'''
Control of fatigue limit state according to SIA 262.
'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2016,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

import math
from materials.fiber_section import createFiberSets
from materials.fiber_section import fiberUtils
from materials.sia262 import shearSIA262
from materials import limit_state_checking_base as lsc
from materials import stressCalc as sc
from postprocess import ControlVars as cv
import geom

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
  return 2.0*sgc #Ver Jiménez Montoya 12.3 (página 244)

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
  vd_max= min(v_0,v_1)
  vd_min= max(v_0,v_1)
  coc= vd_min/vd_max
  retval= 0
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
        secHAParamsCortante= shearSIA262.ShearController(self.limitStateLabel)
        secHAParamsCortante.setSection(section)
        posEsf= geom.Pos3d(N,My,Mz)
        diagInt= e.getProp("diagInt")
        FCflex= diagInt.getCapacityFactor(posEsf)
        controlVars.Mu= My/FCflex
        controlVars.Vu= secHAParamsCortante.calcVu(N,My, controlVars.Mu, Vy)
        
        controlVars.shearLimit= getShearLimit(section,controlVars,controlVars.Vu)
        controlVars.concreteShearCF= getShearCF(controlVars)
        e.setProp(self.limitStateLabel,controlVars)
