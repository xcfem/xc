# -*- coding: utf-8 -*-
'''
Funciones para comprobación de una sección a fisuración según el
artículo 49.2.4 de la EHE-08.
'''
from __future__ import division
from materials.fiber_section import createFiberSets
from materials.fiber_section import fiberUtils
from materials import limit_state_checking_base as lscb
import math

class CrackControl(lscb.CrackControlBaseParameters):
  '''
  Define las propiedades del registro que contiene los parámetros de cálculo
     de la fisuración.'''
  concreteArea= 0.0 #Area de la sección de hormigón.
  fctmFis= 0.0 #Resistencia media del hormigón a tracción.
  tensSRMediaBarrasTracc= 0.0 #Tensión media en las barras traccionadas en fisuración.
  iAreaMaxima= None #Barra traccionada de área máxima.
  diamMaxTracc= 0.0 #Diámetro de la reinforcement traccionada más gruesa.
  EsBarrasTracc= 0.0 #Longitudinal deformation Modulus tensioned rebars.
  eps1= 0.0 #Maximum strain in concrete.
  eps2= 0.0 #Minimum strain in concrete.
  k1= 0.0 #Coeficiente que representa la influencia del diagrama de tracciones.
  k2= 0.5 #Coeficiente de valor 1.0 para carga instantánea no repetida y 0.5 para el resto de los casos.
  depthMecanico= 0.0 #Canto con el que está trabajando la sección.
  widthMecanico= 0.0 #Ancho con el que está trabajando la sección.
  razonAspecto= 0.0 #Cociente entre width y depth.
  hEfMax= 0.0 #Canto máximo del área eficaz.
  AcEfBruta= 0.0 #Área eficaz bruta.
  AcEfNeta= 0.0 #Área eficaz neta.
  E0= 0.0 #Módulo de rigidez tangente del hormigón.
  beta= 1.7 #Coeficiente que relaciona el valor característico de la apertura de fisura con el valor medio y que vale 1.3 para fisuración producida exclusivamente por acciones indirectas y 1.7 en el resto de los casos.
  Wk= 0.0 #Apertura característica de fisuras.

  def printParams(self):
    # Imprime los parámetros de fisuración de la sección.
    print "Num. reinforcement a tracción: ",self.numBarrasTracc,"\n"
    print "Separación entre reinforcement traccionadas; s= ",self.rebarsSpacingTracc," m\n"
    print "Area de las reinforcement traccionadas; As= ",self.areaRebarTracc*1e4," cm2\n"
    print "Area eficaz; AcEf= ",self.AcEfNeta*1e4," cm2\n"
    print "Centro de gravedad de las reinforcement traccionadas; CDG= (",self.yCDGBarrasTracc,",",self.zCDGBarrasTracc,") m\n"
    print "Tensión media en barras traccionadas= ",self.tensMediaBarrasTracc/1e6," MPa\n"
    print "Tensión media en fisuración en barras traccionadas= ",self.tensSRMediaBarrasTracc/1e6," MPa\n"
    print "Deformación máxima en la zona traccionada de hormigón; eps1= ",self.eps1*1e3," por mil.\n"
    print "Deformación mínima en la zona traccionada de hormigón; eps2= ",self.eps2*1e3," por mil.\n"
    print "Influencia del diagrama de tracciones; k1= ",self.k1,"\n"
    print "Canto mecánico; h= ",self.depthMecanico," m\n"
    print "Lever arm; z= ",self.brazoMecanico," m\n"
    print "Ancho mecánico; b= ",self.widthMecanico," m\n"
    print "Razón aspecto; r= ",self.razonAspecto,"\n"
    print "Canto máximo para el área eficaz; hEfMax= ",self.hEfMax," m\n"
    print "Resistencia media a tracción; fctm= ",self.fctmFis/1e6," MPa\n"
    print "Modulo de deformación longitudinal tangente; E0= ",self.E0/1e9," GPa\n"
    print "Apertura característica de fisuras; Wk= ",self.Wk*1e3," mm\n"



  # Calcula la apertura característica de fisura.
  def calcApertCaracFis(self, scc, concreteMatTag, matTagAceroArmar, fctm):
    if(self.rcSets == None):
      self.rcSets= createFiberSets.fiberSectionSetupRC3Sets(scc,concreteMatTag,self.concreteFibersSetName,matTagAceroArmar,self.rebarFibersSetName)
    concrFibers= self.rcSets.concrFibers.fSet
    reinfFibers= self.rcSets.reinfFibers.fSet
    reinforcementTraccion= self.rcSets.tensionFibers

    self.fctmFis= fctm
    self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
    self.numBarrasTracc= self.rcSets.getNumTensionRebars()
    self.Wk= 0.0
    if(self.numBarrasTracc>0):
      scc.calcRecubrimientos(self.tensionedRebarsFiberSetName)
      scc.calcSeparaciones(self.tensionedRebarsFiberSetName)
      self.eps1= concrFibers.getStrainMax()
      self.eps2= max(concrFibers.getStrainMin(),0.0)
      self.k1= (self.eps1+self.eps2)/8/self.eps1
      self.E0= concrFibers[0].getMaterial().getInitialTangent()
      self.concreteArea= concrFibers.getSumaAreas(1)
      self.depthMecanico= scc.getLeverArm()
      self.brazoMecanico= scc.getBrazoMecanico() # z
      self.widthMecanico= scc.getAnchoMecanico()
      self.razonAspecto= self.widthMecanico/self.depthMecanico
      if(self.razonAspecto>1):
        self.hEfMax= self.depthMecanico/4.0 # Pieza tipo losa
      else:
        self.hEfMax= self.depthMecanico/2.0
      self.AcEfNeta= scc.computeFibersEffectiveConcreteArea(self.hEfMax,self.tensionedRebarsFiberSetName,15)

      self.rebarsSpacingTracc= reinforcementTraccion.getAverageDistanceBetweenFibers()
      self.areaRebarTracc= reinforcementTraccion.getArea(1)
      self.yCDGBarrasTracc= reinforcementTraccion.getCdgY()
      self.zCDGBarrasTracc= reinforcementTraccion.getCdgZ()
      self.tensMediaBarrasTracc= reinforcementTraccion.getStressMed()
      self.iAreaMaxima=  fiberUtils.getIMaxPropFiber(reinforcementTraccion,"getArea")
      self.diamMaxTracc= 2*math.sqrt(reinforcementTraccion[self.iAreaMaxima].getArea()/math.pi) 

      self.EsBarrasTracc= reinforcementTraccion[0].getMaterial().getInitialTangent()
      AsBarra= 0.0
      coverBarra= 0.0
      sepBarra= 0.0
      diamBarra= 0.0
      sigmaBarra= 0.0
      sigmaSRBarra= 0.0
      smFisurasBarra= 0.0
      alargMaxBarra= 0.0
      alargMedioBarra= 0.0
      tensSRMediaBarrasTracc= 0.0
      WkBarra= 0.0
      sz= len(reinforcementTraccion)
      for i in range(0,sz):
        barra= reinforcementTraccion[i]
        AsBarra= barra.getArea()
        posBarra= barra.getPos()
        yBarra= posBarra.x
        zBarra= posBarra.y
        sigmaBarra= barra.getMaterial().getStress()
        coverBarra= reinforcementTraccion.getFiberCover(i)
        diamBarra= reinforcementTraccion.getEquivalentDiameterOfFiber(i)
        sigmaSRBarra= reinforcementTraccion.getSigmaSRAtFiber(i,self.E0,self.EsBarrasTracc,self.fctmFis)
        tensSRMediaBarrasTracc+= AsBarra*sigmaSRBarra

        AcEfBarra= reinforcementTraccion.getFiberEffectiveConcreteArea(i)
        sepBarra= min(reinforcementTraccion.getFiberSpacing(i),15*diamBarra)
        smFisurasBarra= 2*coverBarra+0.2*sepBarra+0.4*self.k1*diamBarra*AcEfBarra/AsBarra
        alargMaxBarra= sigmaBarra/self.EsBarrasTracc
        alargMedioBarra= max(1.0-self.k2*(sigmaSRBarra/sigmaBarra)**2,0.4)*alargMaxBarra
        WkBarra=  self.beta*smFisurasBarra*alargMedioBarra
        self.Wk= max(self.Wk,WkBarra)
        # \printParamFisBarra()
      self.tensSRMediaBarrasTracc= self.tensSRMediaBarrasTracc/self.areaRebarTracc
  def check(self,elements,nmbComb):
    ''' Crack control of concrete sections.'''
    print "Postprocessing combination: ",nmbComb,"\n"

    defParamsFisuracion("secHAParamsFisuracion")
    materiales= preprocessor.getMaterialLoader
    concrete= materiales.getMaterial(concreteCode)
    concrTag= concrete.getProp("matTagK")
    concrFctm= concrete.getProp("fctm")
    reinforcement= materiales.getMaterial(codArmadura)
    for e in elements:
      scc= elements.getSeccion()
      Ntmp= scc.N
      MyTmp= scc.My
      MzTmp= scc.Mz
      secHAParamsFisuracion= calcApertCaracFis(concrTag,tagArmadura,concrFctm)
      Wk= secHAParamsFisuracion.Wk
      if(Wk>WkCP):
        WkCP= Wk # Caso pésimo
        HIPCP= nmbComb
        NCP= Ntmp
        MyCP= MyTmp
        MzCP= MzTmp

      

def printParamFisBarra():
# Imprime los parámetros de fisuración de una barra.
  print "\niBarra= ",iBarra,"\n"
  print "Área eficaz Acef= ",AcEfBarra*1e4," cm2\n"
  print "Área barra As= ",AsBarra*1e4," cm2\n"
  print "Pos barra: (",yBarra,",",zBarra,")\n"
  print "Recubrimiento c= ",coverBarra," m\n"
  print "diamBarra fi= ",diamBarra,"\n"
  print "sigmaBarra= ",sigmaBarra/1e6," MPa\n"
  print "sigmaSRBarra= ",sigmaSRBarra/1e6," MPa\n"
  print "Separación s= ",sepBarra," m\n"
  print "k1= ",k1,"\n"
  print "smFisurasBarra= ",smFisurasBarra," m\n"
  print "alargMaxBarra= ",alargMaxBarra*1e3," por mil.\n"
  print "alargMedioBarra= ",alargMedioBarra*1e3," por mil.\n"
  print "WkBarra= ",WkBarra*1e3," mm\n\n"

