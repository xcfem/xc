# -*- coding: utf-8 -*-
from rough_calculations import ng_simple_bending_reinforcement
import math
import xc_base
import geom

from materials.sia262 import concreteSIA262
from materials.sia262 import steelSIA262

def VuNoShearRebars(beton,acier,Nd,Md,AsTrac,b,d):
  # Section shear capacity without shear reinforcement.  
  Mu= ng_simple_bending_reinforcement.Mu(AsTrac,beton.fcd(),acier.fsd(),b,d) 
  return VuNoShearRebarsSIA262(beton,acier,Nd,Md,Mu,b,d)

def VuNoShearRebarsSIA262(beton,acier,Nd,Md,Mu,b,d):
  # Section shear capacity without shear reinforcement. 
  # Simplified method according to document: «Dalles sans étriers soumises
  # à l'effort tranchant» par Aurelio Muttoni (file: beton-VD1V2.pdf)
  h=d/0.9   #mejorar
  if Nd<=0:
    mDd=-Nd*(h/2-d/3)
  else:
    mDd=-Nd*(h/2-(h-d))
  kv= 2.2*(Md-mDd)/(Mu-mDd)
  kd= 1/(1+kv*d)
  taucd= beton.taucd()
  return kd*taucd*b*d

def VuShearRebars90SIA262(Asw,s,acier,z,alpha=math.radians(30)):
  # Shear capacity of shear reinforcement.  
  cot_alpha= 1.0/math.tan(alpha)
  retval= Asw/s*z*acier.fsd()*cot_alpha
  return retval

def VuWithShearRebarsSIA262(beton,acier,Nd,Md,Mu,b,d,Asw,s,z,alpha=math.radians(30)):
  # Section shear capacity without shear reinforcement.  
  Vcu= VuNoShearRebarsSIA262(beton,acier,Nd,Md,Mu,b,d)
  Vsu= VuShearRebars90SIA262(Asw,s,acier,z,alpha)
  return Vcu+Vsu

class ParamsCortante(object):
  '''
  Define las propiedades del registro que contiene los parámetros de cálculo
   de la resistencia a cortante.'''
  hormigon= concreteSIA262.c35_45
  acero= steelSIA262.B500A
  ancho= 0.0
  cantoUtil= 0.0 #Canto útil con el que está trabajando la sección.
  brazoMecanico= 0.0 #Brazo mecánico con el que está trabajando la sección.
  AsTrsv= 0.0
  s= 0.30

  Vcu= 0.0 # Contribución del hormigón a la resistencia al esfuerzo cortante.
  Vsu= 0.0 # Contribución de las armaduras a cortante a la resistencia al esfuerzo cortante.

  def __init__(self,seccionHA):
    #self.hormigon= seccionHA.tipoHormigon Arreglar
    #self.acero= seccionHA.tipoArmadura
    self.ancho= seccionHA.ancho
    self.cantoUtil= 0.9* seccionHA.canto #Mejorar
    self.brazoMecanico= 0.9*self.cantoUtil #Mejorar
    self.AsTrsv= seccionHA.armCortanteZ.getAs()
    self.s= seccionHA.armCortanteZ.espaciamientoRamas

  def calcVcu(self, Nd, Md, Mu):
    ''' Calcula el cortante último de la sección sin armadura de cortante.'''
    self.Vcu= VuNoShearRebarsSIA262(self.hormigon,self.acero,Nd,abs(Md),abs(Mu),self.ancho,self.cantoUtil)
  def calcVsu(self):
    ''' Calcula el cortante último de la sección sin armadura de cortante pasamos
        s= 1.0 porque AsTrsv ya incorpora todas las ramas en un metro.'''
    self.Vsu= VuShearRebars90SIA262(self.AsTrsv,1.0,self.acero,self.brazoMecanico)

  def calcVu(self, Nd, Md, Mu, Vd):
    '''  Calcula el cortante último de la sección.'''
    self.Vcu= 0.0
    self.Vsu= 0.0
    self.calcVcu(Nd,abs(Md),abs(Mu))
    self.calcVsu()
    return self.Vu()

  def Vu(self):
    return self.Vcu+self.Vsu

def defVarsControlVSIA262(elems):
  for e in elems:
    e.setProp("FCCP",0.0) # Caso pésimo
    e.setProp("HIPCP", "")
    e.setProp("NCP", 0.0)
    e.setProp("MyCP", 0.0)
    e.setProp("MzCP", 0.0)
    e.setProp("VyCP", 0.0)
    e.setProp("VzCP", 0.0)
    e.setProp("VsuCP", 0.0)
    e.setProp("VcuCP", 0.0)
    e.setProp("VuCP", 0.0)


def trataResultsCombV(mdlr,nmbComb):
  '''
  Comprobación de las secciones de hormigón frente a cortante.
     XXX Falta tener en cuenta la dirección de las barras de refuerzo
     a cortante.
  '''
  print "Postproceso combinación: ",nmbComb
  # XXX Ignoramos la deformación por torsión.
  elementos= mdlr.getSets.getSet("total").getElements
  for e in elementos:
    e.getResistingForce()
    scc= e.getSection()
    section= scc.getProp("datosSecc")
    secHAParamsCortante= ParamsCortante(section)
    #codHormigon= section.tipoHormigon
    #codArmadura= section.tipoArmadura
    AsTrsv= section.armCortanteY.getAs()
    alpha= section.armCortanteY.angAlphaRamas
    theta= section.armCortanteY.angThetaBielas

    NTmp= scc.getStressResultantComponent("N")
    MyTmp= scc.getStressResultantComponent("My")
    MzTmp= scc.getStressResultantComponent("Mz")
    posEsf= geom.Pos3d(NTmp,MyTmp,MzTmp)
    diagInt= e.getProp("diagInt")
    FCflex= diagInt.getCapacityFactor(posEsf)

    VyTmp= scc.getStressResultantComponent("Vy")
    VzTmp= scc.getStressResultantComponent("Vz")
    Mu= MyTmp/FCflex
    VuTmp= secHAParamsCortante.calcVu(NTmp,MyTmp, Mu, VyTmp)

    if(VuTmp!=0.0):
      FCtmp= abs(VyTmp)/VuTmp
    else:
      FCtmp= 10
    if(FCtmp>=e.getProp("FCCP")):
      e.setProp("FCCP",FCtmp) # Caso pésimo
      e.setProp("HIPCP",nmbComb)
      e.setProp("NCP",NTmp)
      e.setProp("MyCP",MyTmp)
      e.setProp("MzCP",MzTmp)
      e.setProp("MuCP",Mu)
      e.setProp("VyCP",VyTmp)
      e.setProp("VzCP",VzTmp)
      e.setProp("thetaCP",theta)
      e.setProp("VsuCP",secHAParamsCortante.Vsu)
      e.setProp("VcuCP",secHAParamsCortante.Vcu)
      e.setProp("VuCP",VuTmp)
