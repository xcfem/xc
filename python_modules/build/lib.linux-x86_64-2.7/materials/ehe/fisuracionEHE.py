# -*- coding: utf-8 -*-
'''
Funciones para comprobación de una sección a fisuración según el
artículo 49.2.4 de la EHE-08.
'''
from __future__ import division
from materials.fiber_section import creaSetsFibras
from materials.fiber_section import fiberUtils
from materials import crack_control_base as cc
import math

class ParamsFisuracionEHE(cc.CrackControlBaseParameters):
  '''
  Define las propiedades del registro que contiene los parámetros de cálculo
     de la fisuración.'''
  areaHormigon= 0.0 #Area de la sección de hormigón.
  fctmFis= 0.0 #Resistencia media del hormigón a tracción.
  tensSRMediaBarrasTracc= 0.0 #Tensión media en las barras traccionadas en fisuración.
  iAreaMaxima= None #Barra traccionada de área máxima.
  diamMaxTracc= 0.0 #Diámetro de la armadura traccionada más gruesa.
  EsBarrasTracc= 0.0 #Módulo de deformación longitudinal de las barras traccionadas.
  eps1= 0.0 #Deformación máxima en el hormigón.
  eps2= 0.0 #Deformación mínima en el hormigón.
  k1= 0.0 #Coeficiente que representa la influencia del diagrama de tracciones.
  k2= 0.5 #Coeficiente de valor 1.0 para carga instantánea no repetida y 0.5 para el resto de los casos.
  cantoMecanico= 0.0 #Canto con el que está trabajando la sección.
  anchoMecanico= 0.0 #Ancho con el que está trabajando la sección.
  razonAspecto= 0.0 #Cociente entre ancho y canto.
  hEfMax= 0.0 #Canto máximo del área eficaz.
  AcEfBruta= 0.0 #Área eficaz bruta.
  AcEfNeta= 0.0 #Área eficaz neta.
  E0= 0.0 #Módulo de rigidez tangente del hormigón.
  beta= 1.7 #Coeficiente que relaciona el valor característico de la apertura de fisura con el valor medio y que vale 1.3 para fisuración producida exclusivamente por acciones indirectas y 1.7 en el resto de los casos.
  Wk= 0.0 #Apertura característica de fisuras.

  def printParams(self):
    # Imprime los parámetros de fisuración de la sección.
    print "Num. armaduras a tracción: ",self.numBarrasTracc,"\n"
    print "Separación entre armaduras traccionadas; s= ",self.sepBarrasTracc," m\n"
    print "Area de las armaduras traccionadas; As= ",self.areaBarrasTracc*1e4," cm2\n"
    print "Area eficaz; AcEf= ",self.AcEfNeta*1e4," cm2\n"
    print "Centro de gravedad de las armaduras traccionadas; CDG= (",self.yCDGBarrasTracc,",",self.zCDGBarrasTracc,") m\n"
    print "Tensión media en barras traccionadas= ",self.tensMediaBarrasTracc/1e6," MPa\n"
    print "Tensión media en fisuración en barras traccionadas= ",self.tensSRMediaBarrasTracc/1e6," MPa\n"
    print "Deformación máxima en la zona traccionada de hormigón; eps1= ",self.eps1*1e3," por mil.\n"
    print "Deformación mínima en la zona traccionada de hormigón; eps2= ",self.eps2*1e3," por mil.\n"
    print "Influencia del diagrama de tracciones; k1= ",self.k1,"\n"
    print "Canto mecánico; h= ",self.cantoMecanico," m\n"
    print "Brazo mecánico; h= ",self.brazoMecanico," m\n"
    print "Ancho mecánico; b= ",self.anchoMecanico," m\n"
    print "Razón aspecto; r= ",self.razonAspecto,"\n"
    print "Canto máximo para el área eficaz; hEfMax= ",self.hEfMax," m\n"
    print "Resistencia media a tracción; fctm= ",self.fctmFis/1e6," MPa\n"
    print "Modulo de deformación longitudinal tangente; E0= ",self.E0/1e9," GPa\n"
    print "Apertura característica de fisuras; Wk= ",self.Wk*1e3," mm\n"



  # Calcula la apertura característica de fisura.
  def calcApertCaracFis(self, scc, tagDiagHormigon, tagDiagAceroArmar, fctm):
    if(self.rcSets == None):
      self.rcSets= creaSetsFibras.fiberSectionSetupRC3Sets(scc,tagDiagHormigon,self.nmbSetFibrasHormigon,tagDiagAceroArmar,self.nmbSetFibrasArmadura)
    fibrasHormigon= self.rcSets.fibrasHormigon.fSet
    fibrasArmadura= self.rcSets.fibrasArmadura.fSet
    armaduraTraccion= self.rcSets.tractionFibers

    self.fctmFis= fctm
    self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
    self.numBarrasTracc= self.rcSets.getNumBarrasTraccion()
    self.Wk= 0.0
    if(self.numBarrasTracc>0):
      scc.calcRecubrimientos(self.nmbSetFibrasArmaduraTraccion)
      scc.calcSeparaciones(self.nmbSetFibrasArmaduraTraccion)
      self.eps1= fibrasHormigon.getStrainMax()
      self.eps2= max(fibrasHormigon.getStrainMin(),0.0)
      self.k1= (self.eps1+self.eps2)/8/self.eps1
      self.E0= fibrasHormigon[0].getMaterial().getInitialTangent()
      self.areaHormigon= fibrasHormigon.getSumaAreas(1)
      self.cantoMecanico= scc.getCantoMecanico()
      self.brazoMecanico= scc.getBrazoMecanico() # z
      self.anchoMecanico= scc.getAnchoMecanico()
      self.razonAspecto= self.anchoMecanico/self.cantoMecanico
      if(self.razonAspecto>1):
        self.hEfMax= self.cantoMecanico/4.0 # Pieza tipo losa
      else:
        self.hEfMax= self.cantoMecanico/2.0
      self.AcEfNeta= scc.calcAcEficazFibras(self.hEfMax,self.nmbSetFibrasArmaduraTraccion,15)

      self.sepBarrasTracc= armaduraTraccion.getDistMediaFibras()
      self.areaBarrasTracc= armaduraTraccion.getArea(1)
      self.yCDGBarrasTracc= armaduraTraccion.getCdgY()
      self.zCDGBarrasTracc= armaduraTraccion.getCdgZ()
      self.tensMediaBarrasTracc= armaduraTraccion.getStressMed()
      self.iAreaMaxima=  fiberUtils.getIMaxPropFiber(armaduraTraccion,"getArea")
      self.diamMaxTracc= 2*math.sqrt(armaduraTraccion[self.iAreaMaxima].getArea()/math.pi) 

      self.EsBarrasTracc= armaduraTraccion[0].getMaterial().getInitialTangent()
      AsBarra= 0.0
      recubBarra= 0.0
      sepBarra= 0.0
      diamBarra= 0.0
      sigmaBarra= 0.0
      sigmaSRBarra= 0.0
      smFisurasBarra= 0.0
      alargMaxBarra= 0.0
      alargMedioBarra= 0.0
      tensSRMediaBarrasTracc= 0.0
      WkBarra= 0.0
      sz= len(armaduraTraccion)
      for i in range(0,sz):
        barra= armaduraTraccion[i]
        AsBarra= barra.getArea()
        posBarra= barra.getPos()
        yBarra= posBarra.x
        zBarra= posBarra.y
        sigmaBarra= barra.getMaterial().getStress()
        recubBarra= armaduraTraccion.getRecubrimientoFibra(i)
        diamBarra= armaduraTraccion.getDiamEqFibra(i)
        sigmaSRBarra= armaduraTraccion.getSigmaSRFibra(i,self.E0,self.EsBarrasTracc,self.fctmFis)
        tensSRMediaBarrasTracc+= AsBarra*sigmaSRBarra

        AcEfBarra= armaduraTraccion.getAcEficazFibra(i)
        sepBarra= min(armaduraTraccion.getSeparacionFibra(i),15*diamBarra)
        smFisurasBarra= 2*recubBarra+0.2*sepBarra+0.4*self.k1*diamBarra*AcEfBarra/AsBarra
        alargMaxBarra= sigmaBarra/self.EsBarrasTracc
        alargMedioBarra= max(1.0-self.k2*(sigmaSRBarra/sigmaBarra)**2,0.4)*alargMaxBarra
        WkBarra=  self.beta*smFisurasBarra*alargMedioBarra
        self.Wk= max(self.Wk,WkBarra)
        # \printParamFisBarra()
      self.tensSRMediaBarrasTracc= self.tensSRMediaBarrasTracc/self.areaBarrasTracc

      

def printParamFisBarra():
# Imprime los parámetros de fisuración de una barra.
  print "\niBarra= ",iBarra,"\n"
  print "Área eficaz Acef= ",AcEfBarra*1e4," cm2\n"
  print "Área barra As= ",AsBarra*1e4," cm2\n"
  print "Pos barra: (",yBarra,",",zBarra,")\n"
  print "Recubrimiento c= ",recubBarra," m\n"
  print "diamBarra fi= ",diamBarra,"\n"
  print "sigmaBarra= ",sigmaBarra/1e6," MPa\n"
  print "sigmaSRBarra= ",sigmaSRBarra/1e6," MPa\n"
  print "Separación s= ",sepBarra," m\n"
  print "k1= ",k1,"\n"
  print "smFisurasBarra= ",smFisurasBarra," m\n"
  print "alargMaxBarra= ",alargMaxBarra*1e3," por mil.\n"
  print "alargMedioBarra= ",alargMedioBarra*1e3," por mil.\n"
  print "WkBarra= ",WkBarra*1e3," mm\n\n"

def trataResultsCombFISEHE(mdlr,nmbComb):
  # Comprobación de las secciones de hormigón frente a fisuración.
  print "Postproceso combinación: ",nmbComb,"\n"

  defParamsFisuracion("secHAParamsFisuracion")
  materiales= mdlr.getMaterialLoader
  hormigon= materiales.getMaterial(codHormigon)
  tagHorm= hormigon.getProp("tagDiagK")
  fctmHorm= hormigon.getProp("fctm")
  armadura= materiales.getMaterial(codArmadura)
  elementos= mdlr.getElementLoader
  for e in elementos:
    scc= elementos.getSeccion()
    Ntmp= scc.N
    MyTmp= scc.My
    MzTmp= scc.Mz
    secHAParamsFisuracion= calcApertCaracFis(tagHorm,tagArmadura,fctmHorm)
    Wk= secHAParamsFisuracion.Wk
    if(Wk>WkCP):
      WkCP= Wk # Caso pésimo
      HIPCP= nmbComb
      NCP= Ntmp
      MyCP= MyTmp
      MzCP= MzTmp
