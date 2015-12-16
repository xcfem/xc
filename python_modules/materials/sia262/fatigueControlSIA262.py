# -*- coding: utf-8 -*-
'''
Funciones para comprobación de una sección a fisuración según el
artículo 49.2.4 de la EHE-08.
'''
from materials.fiber_section import createFiberSets
from materials.fiber_section import fiberUtils
from materials.sia262 import shearSIA262
from materials import crack_control_base as cc
from materials import stressCalc as sc
import math
import geom

def defVarsControl(elems):
  for e in elems:
    e.setProp("sg_sPos0",0)
    e.setProp("sg_sNeg0",0)
    e.setProp("sg_c0",0)
    e.setProp("N0",0)
    e.setProp("My0",0)
    e.setProp("Mz0",0)
    e.setProp("Vy0",0)
    e.setProp("Vz0",0)
    e.setProp("sg_sPos1",0)
    e.setProp("sg_sNeg1",0)
    e.setProp("sg_c1",0)
    e.setProp("N1",0)
    e.setProp("My1",0)
    e.setProp("Mz1",0)
    e.setProp("Vy1",0)
    e.setProp("Vz1",0)


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

def limitFatigueBeton(sccData,kc,sigma_c0,sigma_c1):
  fcd= sccData.tipoHormigon.fcd()
  #print "sigma_c0= ", sigma_c0/1e6, " sigma_c1= ", sigma_c1/1e6
  sg_max= min(min(sigma_c0,sigma_c1),0.0) # No possitive sttresses.
  sg_min= min(max(sigma_c0,sigma_c1),0.0) # No possitive sttresses.
  return min(-0.5*kc*fcd+0.45*abs(sg_min),-0.9*kc*fcd)

def limitShear(sccData,v_0,v_1,vu):
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

def trataResultsComb(preprocessor,nmbComb):
  # Comprobación de las secciones de hormigón frente a fatiga estimando la tensión en la armadura.
  print "Postproceso combinación: ",nmbComb,"\n"

  index= int(nmbComb[-1])

  elementos= preprocessor.getSets.getSet("total").getElements
  for e in elementos:
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

    # sigma_sPos= estimateSteelStressPos(datosScc, N, My)
    # sigma_sNeg= estimateSteelStressNeg(datosScc, N, My)
    # sigma_s= max(sigma_sPos,sigma_sNeg)
    # #sigma_c= estimateSigmaC(datosScc, sigma_sPos, sigma_sNeg)
    # sigma_c= estimateSigmaCPlanB(datosScc, N, My)

    # if(sigma_c==0.0):
    #   print "****** sigma_sPos=", sigma_sPos/1e6, "sigma_sNeg=", sigma_sNeg/1e6, " sigma_c= ", sigma_c, " N= ",N/1e3, "M= ", My/1e3 
    if(index==0):
      e.setProp("sg_sPos0",sigma_sPos)
      e.setProp("sg_sNeg0",sigma_sNeg)
      e.setProp("sg_c0",sigma_c)
      e.setProp("N0",N)
      e.setProp("My0",My)
      e.setProp("Mz0",Mz)
      e.setProp("Vy0",Vy)
      e.setProp("Vz0",Vz)
    else:
      e.setProp("sg_sPos1",sigma_sPos)
      e.setProp("sg_sNeg1",sigma_sNeg)
      e.setProp("sg_c1",sigma_c)
      e.setProp("N1",N)
      e.setProp("My1",My)
      e.setProp("Mz1",Mz)
      e.setProp("Vy1",Vy)
      e.setProp("Vz1",Vz)
      kc= 1.0 #XXX  SIA 262 4.2.1.7
      lim_sg_c= limitFatigueBeton(datosScc,kc,e.getProp("sg_c0"),sigma_c)
      #print "lim_sg_c= ", lim_sg_c/1e6
      sgc0= abs(min(e.getProp("sg_c0"),0.0))
      sgc1= abs(min(sigma_c,0.0))
      fc_sg_c= max(sgc0,sgc1)/lim_sg_c
      #print "fc_sg_c= ",fc_sg_c
      e.setProp("lim_sg_c",lim_sg_c)
      e.setProp("fc_sg_c",fc_sg_c)
      section= scc.getProp("datosSecc")
      secHAParamsCortante= shearSIA262.ParamsCortante(section)
      posEsf= geom.Pos3d(N,My,Mz)
      diagInt= e.getProp("diagInt")
      FCflex= diagInt.getCapacityFactor(posEsf)
      Mu= My/FCflex
      Vu= secHAParamsCortante.calcVu(N,My, Mu, Vy)
      Vy0= e.getProp("Vy0")
      Vy1= Vy
      lim_v= limitShear(section,Vy0,Vy1,Vu)
      fc_v= max(abs(Vy0),abs(Vy1))/lim_v
      e.setProp("Mu",Mu)
      e.setProp("Vu",Vu)
      e.setProp("lim_v",lim_v)
      e.setProp("fc_v",fc_v)
