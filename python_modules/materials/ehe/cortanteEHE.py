# -*- coding: utf-8 -*-
'''
Funciones para la comprobación a cortante según el artículo
 44.2.3.2 de EHE-08
'''
from __future__ import division
import math
from materials.fiber_section import createFiberSets
from materials.ehe import comprobVEHE08
from materials.ehe import torsionEHE
import geom
import sys
from postprocess import control_vars as cv
from materials import limit_state_checking_base as lsc

class ShearControllerEHE(lsc.LimitStateControllerBase):
  '''Parameters for shear control with EHE.'''
  def __init__(self,limitStateLabel):
    super(ShearControllerEHE,self).__init__(limitStateLabel)
    self.concreteFibersSetName= "concrete" #Name of the concrete fibers set.
    self.rebarFibersSetName= "reinforcement" #Name of the rebar fibers set.
    self.tensionedRebarsFiberSetName= "reinforcementTraccion" #Name of the tensioned rebars set.
    self.hayMom= False #Verdadero si la sección está sometida a momento.
    self.fcvH= 0.0 #Resistencia efectiva del hormigón a cortante.
    self.fckH= 0.0 #Valor característico de la resistencia del hormigón a compresión.
    self.fcdH= 0.0 #Valor de cálculo de la resistencia del hormigón a compresión.
    self.fctdH= 0.0 #Valor de cálculo de la resistencia del hormigón a tracción.
    self.gammaC= 0.0 # Partial safety factor for concrete.
    self.fydS= 0.0 #Valor de cálculo de la resistencia del acero de armar a tracción.
    self.depthUtil= 0.0 #Canto útil con el que está trabajando la sección.
    self.brazoMecanico= 0.0 #Lever arm con el que está trabajando la sección.
    self.strutWidth= 0.0 #Compressed strut width «b0».
    self.I= 0.0 #Momento of inertia of the section with respect to the neutral axis in régimen elástico.
    self.S= 0.0 #First moment of area of the section with respect to the neutral axis in régimen elástico.
    self.concreteArea= 0.0 #Area de la sección de hormigón.
    self.numBarrasTraccion= 0 #Número de barras sometidas a tracción.
    self.areaRebarTracc= 0.0 #Area total de las barras traccionadas.
    self.eps1= 0.0 #Maximum strain in concrete.
    self.concreteAxialForce= 0.0 #Esfuerzo axil soportado por el hormigón.
    self.E0= 0.0 #Módulo de rigidez tangente del hormigón.

    self.alphaL= 1.0 #Factor que depende de la transferencia de pretensado.
    self.AsTrsv= 0.0 #Área de la reinforcement de cortante.
    self.alpha= math.radians(90) #Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1 EHE-08).
    self.theta= math.radians(45) #Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    self.thetaMin= math.atan(0.5) #Minimal value of the theta angle.
    self.thetaMax= math.atan(2) #Maximal value of the theta angle.

    self.thetaFisuras= 0.0 #Angulo de las fisuras con el eje de la pieza.
    self.Vcu= 0.0 # Contribución del hormigón a la resistencia al esfuerzo cortante.
    self.Vsu= 0.0 # Contribución de las reinforcement a cortante a la resistencia al esfuerzo cortante.
    self.Vu1= 0.0 #Agotamiento por compresión oblicua del alma.
    self.Vu2= 0.0 #Agotamiento por tracción en el alma.
    self.Vu= 0.0 #Cortante último de la sección.

  def calcVuEHE08NoAt(self, preprocessor, scc, concrete, aceroArmar):
    ''' Calcula el cortante último de la sección sin reinforcement de cortante.
     XXX Falta considerar la reinforcement activa.
     matTagAceroArmar: Identificador del material empleado para modelizar el acero de armar.
     concrete: Parámetros para modelizar el hormigón.
     aceroArmar: Parámetros para modelizar el acero de armar.'''
    self.concreteMatTag= concrete.matTagD
    self.fckH= abs(concrete.fck)
    self.fcdH= abs(concrete.fcd())
    self.fctdH= concrete.fctd()
    self.gammaC= concrete.gmmC
    self.matTagAceroArmar= aceroArmar.matTagD
    self.fydS= aceroArmar.fyd()

    if(not scc.hasProp("rcSets")):
      scc.setProp("rcSets", createFiberSets.fiberSectionSetupRC3Sets(scc,self.concreteMatTag,self.concreteFibersSetName,self.matTagAceroArmar,self.rebarFibersSetName))
    rcSets= scc.getProp("rcSets")

    concrFibers= rcSets.concrFibers.fSet
    self.concreteArea= rcSets.getConcreteArea(1)
    if(self.concreteArea<1e-6):
      errMsg= "concrete area too smail; Ac= " + str(self.concreteArea) + " m2\n"
      sys.stderr.write(errMsg)
    else:
      reinfFibers= rcSets.reinfFibers.fSet
      reinforcementTraccion= rcSets.tensionFibers
      self.hayMom= scc.isSubjectedToBending(0.1)
      self.numBarrasTraccion= rcSets.getNumTensionRebars()
      if(self.hayMom):
        self.eps1= rcSets.getMaxConcreteStrain()
        self.E0= rcSets.getConcreteInitialTangent()
        self.concreteAxialForce= rcSets.getConcreteCompression()
        self.strutWidth= scc.getCompressedStrutWidth() # b0
        if((self.E0*self.eps1)<self.fctdH): # Sección no fisurada
          self.I= scc.getHomogenizedI(self.E0)
          self.S= scc.getSPosHomogeneizada(self.E0)
          self.Vu2= comprobVEHE08.getVu2EHE08NoAtNoFis(self.fctdH,self.I,self.S,self.strutWidth,self.alphaL,self.concreteAxialForce,self.concreteArea)
        else: # Sección fisurada
          self.depthUtil= scc.getCantoUtil() # d
          if(self.numBarrasTraccion>0):
            self.areaRebarTracc= reinforcementTraccion.getArea(1)
          else:
            self.areaRebarTracc= 0.0
          self.Vu2= comprobVEHE08.getVu2EHE08NoAtSiFis(self.fckH,self.fcdH,self.gammaC,self.concreteAxialForce,self.concreteArea,self.strutWidth,self.depthUtil,self.areaRebarTracc,0.0)
        self.Vcu= self.Vu2
        self.Vsu= 0.0
        self.Vu1= -1.0
        self.Vu= self.Vu2
      else: # Sección no fisurada
        axis= scc.getInternalForcesAxis()
        self.I= scc.getFibers().getHomogenizedSectionIRelToLine(self.E0,axis)
        self.S= scc.getFibers().getSPosHomogenizedSection(self.E0,geom.HalfPlane2d(axis))
        self.Vu2= comprobVEHE08.getVu2EHE08NoAtNoFis(self.fctdH,self.I,self.S,self.strutWidth,self.alphaL,self.concreteAxialForce,self.concreteArea)

  def calcVuEHE08SiAt(self, preprocessor, scc, paramsTorsion, concrete, aceroArmar, Nd, Md, Vd, Td):
    ''' Calcula el cortante último de la sección CON reinforcement de cortante.
     XXX Falta considerar la reinforcement activa.
     matTagAceroArmar: Identificador del material empleado para modelizar el acero de armar.
     concrete: Nombre del material empleado para modelizar el hormigón.
     aceroArmar: Nombre del material empleado para modelizar el acero de armar.
     Nd: Valor de cálculo del axil (aquí positivo si es de tracción)
     Md: Valor absoluto del momento de cálculo.
     Vd: Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
     Td: Torsor de cálculo. '''
    self.VuAe= paramsTorsion.Ae()
    self.Vuue= paramsTorsion.ue()

    self.concreteMatTag= concrete.matTagD
    self.fckH= abs(concrete.fck)
    self.fcdH= abs(concrete.fcd())
    self.fctdH= abs(concrete.fctd())
    self.gammaC= abs(concrete.gmmC)
    self.matTagAceroArmar= aceroArmar.matTagD
    self.fydS= aceroArmar.fyd()

    createFiberSets.fiberSectionSetupRC3Sets(scc,self.concreteMatTag,self.concreteFibersSetName,self.matTagAceroArmar,self.rebarFibersSetName)
    concrFibers= scc.getFiberSets()[self.concreteFibersSetName]
    reinfFibers= scc.getFiberSets()[self.rebarFibersSetName]
    reinforcementTraccion= scc.getFiberSets()[self.tensionedRebarsFiberSetName]

    self.hayMom= scc.isSubjectedToBending(0.1)
    self.numBarrasTraccion= reinforcementTraccion.getNumFibers()
    self.concreteArea= concrFibers.getArea(1)
    if(self.concreteArea<1e-6):
      errMsg= "concrete area too smail; Ac= " + str(self.concreteArea) + " m2\n"
      sys.stderr.write(errMsg)
    else:
      if(self.hayMom):
        self.eps1= concrFibers.getStrainMax()
        self.E0= concrFibers[0].getMaterial().getInitialTangent()
        self.concreteAxialForce= concrFibers.ResultanteComp()
        self.modElastArmadura= reinfFibers[0].getMaterial().getInitialTangent()
        self.strutWidth= scc.getCompressedStrutWidth() # b0
        self.depthUtil= scc.getCantoUtil() # d
        self.brazoMecanico= scc.getBrazoMecanico() # z
        if(self.numBarrasTraccion>0):
          self.areaRebarTracc= reinforcementTraccion.getArea(1)
        else:
          self.areaRebarTracc= 0.0
        self.thetaFisuras= comprobVEHE08.getAnguloInclinacionFisurasEHE08(Nd,Md,Vd,Td,self.brazoMecanico,self.areaRebarTracc,0.0,self.modElastArmadura,0.0,0.0,self.VuAe,self.Vuue)
        self.Vcu= comprobVEHE08.getVcuEHE08(self.fckH,self.fcdH,self.gammaC,self.concreteAxialForce,self.concreteArea,self.strutWidth,self.depthUtil,self.brazoMecanico,self.areaRebarTracc,0.0,self.theta,Nd,Md,Vd,Td,self.modElastArmadura,0.0,0.0,self.VuAe,self.Vuue)
        self.Vu1= comprobVEHE08.getVu1EHE08(self.fckH,self.fcdH,self.concreteAxialForce,self.concreteArea,self.strutWidth,self.depthUtil,self.alpha,self.theta)
        self.Vsu= comprobVEHE08.getVsuEHE08(self.brazoMecanico,self.alpha,self.theta,self.AsTrsv,self.fydS)
        self.Vu2= self.Vcu+self.Vsu
        self.Vu= min(self.Vu1,self.Vu2)
      else: # Sección no fisurada
        sys.stderr.write("La comprobación del cortante sin momento no está implementada.")

  def calcVuEHE08(self, preprocessor, scc, nmbParamsTorsion, concrete, aceroArmar, Nd, Md, Vd, Td):
    '''  Calcula el cortante último de la sección.
     XXX Falta considerar la reinforcement activa.
     concrete: parameters to model concrete.
     aceroArmar: parameters to model rebar's steel.
     Nd: Valor de cálculo del axil (aquí positivo si es de tracción)
     Md: Valor absoluto del momento de cálculo.
     Vd: Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
     Td: Torsor de cálculo.'''
    if(self.AsTrsv==0):
      self.calcVuEHE08NoAt(preprocessor,scc,concrete,aceroArmar)
    else:
      self.calcVuEHE08SiAt(preprocessor,scc,nmbParamsTorsion,concrete,aceroArmar,Nd,Md,Vd,Td)


  def check(self,elements,nmbComb):
    '''
    Comprobación de las secciones de hormigón frente a cortante.
       XXX Falta tener en cuenta la dirección de las barras de refuerzo
       a cortante.
    '''
    print "Postprocessing combination: ",nmbComb
    secHAParamsTorsion=  torsionEHE.TorsionParameters()
    # XXX Ignore torsional deformation.
    secHAParamsTorsion.ue= 0
    secHAParamsTorsion.Ae= 1
    elementos= preprocessor.getSets.getSet("total").getElements
    for e in elementos:
      scc= e.getSection()
      idSection= e.getProp("idSection")
      section= scc.getProp("datosSecc")
      concreteCode= section.concrType
      codArmadura= section.reinfSteelType
      AsTrsv= section.shReinfY.getAs()
      alpha= section.shReinfY.angAlphaShReinf
      theta= section.shReinfY.angThetaConcrStruts
      NTmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      MzTmp= scc.getStressResultantComponent("Mz")
      MTmp= math.sqrt((MyTmp)**2+(MzTmp)**2)
      VyTmp= scc.getStressResultantComponent("Vy")
      VzTmp= scc.getStressResultantComponent("Vz")
      VTmp= math.sqrt((VyTmp)**2+(VzTmp)**2)
      TTmp= scc.getStressResultantComponent("Mx")
      secHAParamsCortante.calcVuEHE08(preprocessor,scc,secHAParamsTorsion,concreteCode,codArmadura,NTmp,MTmp,VTmp,TTmp)

      if(secHAParamsCortante.Vu<VTmp):
        theta= max(secHAParamsCortante.thetaMin,min(secHAParamsCortante.thetaMax,secHAParamsCortante.thetaFisuras))
        secHAParamsCortante.calcVuEHE08(preprocessor,scc,secHAParamsTorsion,concreteCode,codArmadura,NTmp,MTmp,VTmp,TTmp)
      if(secHAParamsCortante.Vu<VTmp):
        theta= (secHAParamsCortante.thetaMin+secHAParamsCortante.thetaMax)/2.0
        secHAParamsCortante.calcVuEHE08(preprocessor,scc,secHAParamsTorsion,concreteCode,codArmadura,NTmp,MTmp,VTmp,TTmp)
      if(secHAParamsCortante.Vu<VTmp):
        theta= 0.95*secHAParamsCortante.thetaMax
        secHAParamsCortante.calcVuEHE08(preprocessor,scc,secHAParamsTorsion,concreteCode,codArmadura,NTmp,MTmp,VTmp,TTmp)
      if(secHAParamsCortante.Vu<VTmp):
        theta= 1.05*secHAParamsCortante.thetaMin
        secHAParamsCortante.calcVuEHE08(preprocessor,scc,secHAParamsTorsion,concreteCode,codArmadura,NTmp,MTmp,VTmp,TTmp)
      VuTmp= secHAParamsCortante.Vu
      if(VuTmp!=0.0):
        FCtmp= VTmp/VuTmp
      else:
        FCtmp= 1e99
      if(FCtmp>=e.getProp("ULS_shear").CF):
        e.setProp("ULS_shear",cv.RCShearControlVars(idSection,nmbComb,FCtmp,NTmp,MyTmp,MzTmp,Mu,VyTmp,VzTmp,theta,secHAParamsCortante.Vcu,secHAParamsCortante.Vsu,VuTmp)) # Worst case
