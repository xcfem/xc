# -*- coding: utf-8 -*-
from __future__ import division
''' Limit state checking according to structural concrete spanish standard EHE-08.'''

import math
import xc_base
import geom
from materials.fiber_section import createFiberSets
from materials.fiber_section import fiberUtils
from materials import limit_state_checking_base as lscb

# Checking normal stresses of a reinforced concrete section.

def cumpleFlexotraccion(epsCMin, epsSMax):
  '''Comprobación en flexotracción.'''
  retval= False
  if(epsCMin>=-3.5e-3): # Minimum concrete strain OK.
    if(epsSMax<=10e-3): # Maximum reinforcing steel strain OK.
      retval= True
  return retval

def aprovFlexotraccion(epsCMin,epsSMax):
  '''Aprovechamiento sección en flexotracción.'''
  return (max(epsCMin/-3.5e-3,epsSMax/10e-3))

def cumpleCompresionCompuesta(epsCMin,epsCMax):
  '''Comprobación en compresión simple o compuesta.'''
  retval= False
  if(epsCMin>=(-3.5e-3 - 3/4*epsCMax)): # Si cumple deformación mínima del hormigón.
    if(epsCMax>=-2e-3): # Si cumple deformación máxima del hormigón.
      retval= True
  return retval

def aprovCompresionCompuesta(epsCMin,epsCMax):
  '''Aprovechamiento sección en compresión simple o compuesta.'''
  return (max(epsCMin/(-3.5e-3 - 3/4*epsCMax),epsCMax/-2e-3))


def cumpleTraccionCompuesta(epsSMax):
  '''Comprobación en tracción simple o compuesta.'''
  retval= False
  if(epsSMax<=10e-3): # Maximum reinforcing steel strain OK.
    retval= True
  return retval

def aprovTraccionCompuesta(epsSMax):
  '''Aprovechamiento sección en tracción simple o compuesta.'''
  return (epsSMax/10e-3)

def aprovTN(tipoSol, epsCMin, epsCMax, epsSMax):
  retval= 0.0
  if(tipoSol==1): # Tracción simple o compuesta.
    retval= aprovTraccionCompuesta(epsSMax)
  else:
    if(tipoSol==2): # Flexotracción.
      retval= aprovFlexotraccion(epsCMin,epsSMax)
    else:
      if(tipoSol==3): # Compresión simple o compuesta.
        retval= aprovCompresionCompuesta(epsCMin,epsCMax)
      else:
        retval= -100
  return retval

class CrackControl(lscb.CrackControlBaseParameters):
  '''
  Define the properties that will be needed for crack control checking
  as in clause 49.2.4 of EHE-08.'''

  def __init__(self,limitStateLabel):
    super(CrackControl,self).__init__(limitStateLabel)
    self.concreteArea= 0.0 #Concrete section area.
    self.fctmFis= 0.0 #Resistencia media del hormigón a tracción.
    self.tensSRMediaBarrasTracc= 0.0 #Tensión media en las barras traccionadas en fisuración.
    self.iAreaMaxima= None #Barra traccionada de área máxima.
    self.diamMaxTracc= 0.0 #Diámetro de la reinforcement traccionada más gruesa.
    self.EsBarrasTracc= 0.0 #Longitudinal deformation Modulus tensioned rebars.
    self.eps1= 0.0 #Maximum strain in concrete.
    self.eps2= 0.0 #Minimum strain in concrete.
    self.k1= 0.0 #Coeficiente que representa la influencia del diagrama de tracciones.
    self.k2= 0.5 #Coeficiente de valor 1.0 para carga instantánea no repetida y 0.5 para el resto de los casos.
    self.depthMecanico= 0.0 #Canto con el que está trabajando la sección.
    self.widthMecanico= 0.0 #Ancho con el que está trabajando la sección.
    self.razonAspecto= 0.0 #Cociente entre width y depth.
    self.hEfMax= 0.0 #Canto máximo del área eficaz.
    self.AcEfBruta= 0.0 #Área eficaz bruta.
    self.AcEfNeta= 0.0 #Área eficaz neta.
    self.E0= 0.0 #Módulo de rigidez tangente del hormigón.
    self.beta= 1.7 #Coeficiente que relaciona el valor característico de la apertura de fisura con el valor medio y que vale 1.3 para fisuración producida exclusivamente por acciones indirectas y 1.7 en el resto de los casos.
    self.Wk= 0.0 #Apertura característica de fisuras.
    
  def printParams(self):
    # Imprime los parámetros de fisuración de la sección.
    print "Num. reinforcement a tracción: ",self.numBarrasTracc,"\n"
    print "Spacement of the tensioned bars; s= ",self.rebarsSpacingTracc," m\n"
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
  def calcApertCaracFis(self, scc, concreteMatTag, reinfSteelMaterialTag, fctm):
    if(self.rcSets == None):
      self.rcSets= createFiberSets.fiberSectionSetupRC3Sets(scc,concreteMatTag,self.concreteFibersSetName,reinfSteelMaterialTag,self.rebarFibersSetName)
    concrFibers= self.rcSets.concrFibers.fSet
    reinfFibers= self.rcSets.reinfFibers.fSet
    reinforcementTraccion= self.rcSets.tensionFibers

    self.fctmFis= fctm
    self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
    self.numBarrasTracc= self.rcSets.getNumTensionRebars()
    self.Wk= 0.0
    if(self.numBarrasTracc>0):
      scc.computeCovers(self.tensionedRebarsFiberSetName)
      scc.computeSpacement(self.tensionedRebarsFiberSetName)
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
  print "Cover c= ",coverBarra," m\n"
  print "diamBarra fi= ",diamBarra,"\n"
  print "sigmaBarra= ",sigmaBarra/1e6," MPa\n"
  print "sigmaSRBarra= ",sigmaSRBarra/1e6," MPa\n"
  print "Bar spacement s= ",sepBarra," m\n"
  print "k1= ",k1,"\n"
  print "smFisurasBarra= ",smFisurasBarra," m\n"
  print "alargMaxBarra= ",alargMaxBarra*1e3," por mil.\n"
  print "alargMedioBarra= ",alargMedioBarra*1e3," por mil.\n"
  print "WkBarra= ",WkBarra*1e3," mm\n\n"


class TorsionParameters(object):
  '''Methods for checking reinforced concrete section under torsion according to
     clause 45.1 or EHE-08.'''
  def __init__(self):
    self.h0= 0.0  # Real wall thickess.
    self.c= 0.0  # Longitudinal reinforcement concrete cover.

    self.crossSectionContour= geom.Poligono2d()  # Cross section contour.
    self.lineaMedia=  geom.Poligono2d() # Polygon defined by the midline of the effective hollow section.
    self.lineaInt=  geom.Poligono2d() # Polygon defined by the interior contour of the effective hollow section.
    self.seccionHuecaEficaz= geom.PoligonoConAgujeros2d() # Effective hollow section contour
  def A(self):
    return self.crossSectionContour.getArea()
  def u(self):
    return self.crossSectionContour.getPerimetro()
  def he(self):
    return max(2*self.c,min(self.A()/self.u(),self.h0))
  def Ae(self):
    return self.lineaMedia.getArea()
  def ue(self):
    return self.lineaMedia.getPerimetro()

def calcParamsSeccionHuecaEficaz(geomSeccion, h0, c):
  '''
  Calcula los parámetros de torsión que se deducen
   de la sección hueca eficaz. No es válido si la sección no es convexa.
  gmSectionName: Identificador de la definición geométrica de la sección.
  nmbParamsTorsión: Identificador del registro que contiene los parámetros de cálculo
                de la resistencia a torsión.
  h0: Real thickness of the section wall.
  c: Cover of the longitudinal reinforcement.
  '''
  retval= TorsionParameters()
  retval.h0= h0
  retval.c= c
  retval.crossSectionContour= geomSeccion.getRegionsContour()
  he= retval.he()
  retval.lineaMedia= retval.crossSectionContour.offset(-he/2)
  retval.lineaInt= retval.crossSectionContour.offset(-he)
  retval.seccionHuecaEficaz.contour(retval.crossSectionContour)
  retval.seccionHuecaEficaz.addHole(retval.lineaInt)
  return retval



# Cuantía geométrica mínima en pilares.
def cuantiaGeomMinPilares(Ac):
  return 0.004*Ac 

# Cuantía mecánica mínima en pilares.
def cuantiaMecMinPilares(Nd, fyd):
  return 0.1*Nd/fyd 

# Cuantía máxima en pilares.
def cuantiaMaxPilares(Ac, fcd, fyd):
  return min((fcd*Ac)/fyd,0.08*Ac)
 
# Verificación de la cuantía de la reinforcement longitudinal en pilares.
def verifCuantiaPilar(Ac, As, fcd, fyd, Nd):
  ctMinGeom= cuantiaGeomMinPilares(Ac)
  if(As<ctMinGeom):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, inferior a la mínima geométrica: ",ctMinGeom*1e4," cm2\n"
  ctMinMec= cuantiaMecMinPilares(Nd,fyd)
  if(As<ctMinMec):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, inferior a la mínima mecánica: ",ctMinMec*1e4," cm2\n"
  ctMax= cuantiaMaxPilares(Ac,fcd,fyd)
  if(As>ctMax):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, superior a la máxima: ",ctMax*1e4," cm2\n"


class ConcreteCorbel(object):
    '''Concrete corbel as in EHE-08 design code.'''
    def __init__(self,concrete, steel, jointType):
        '''
        Constructor.

        :param concrete: concrete material for corbel.
        :param concrete: steel material for corbel reinforcement.
        :param jointType: corbel to column joint quality
        ("monolitica", "junta" o "junta_debil").
        '''
        self.concrete= concrete
        self.steel= steel
        self.jointType= jointType
        
    def getCotgStrutAngle(self):
        '''getCotgStrutAngle()
 
          Return the cotangent of the angle between the concrete
          compressed strut an the vertical according to 
          clause 64.1.2.1 de EHE-08.
        '''
        if self.jointType=="monolitica":
            retval=1.4
        elif self.jointType=="junta":
            retval=1.0
        elif self.jointType=="junta_debil":
            retval=0.6
        return retval

    def getCantoUtilMinimo(self, a):
        '''getCantoUtilMinimo(self, a)

        :param a: Distancia (m) entre el eje de la carga aplicada y la sección
        de empotramiento de la ménsula (ver figura 64.1.2 de EHE-08).

        Devuelve el depth útil mínimo de la ménsula según
        el apartado 64.1.2.1 de EHE-08.
        '''
        return a*self.getCotgStrutAngle()/0.85

    def getTraccionMainReinforcement(self, Fv,Fh):
        '''getTraccionMainReinforcement(self, Fv,Fh)

        :param Fv: Vertical load on the corbel, positive downwards (N).
        :param Fh: Horizontal load on the corbel, positive outwards (N).

        Devuelve la tracción en la armadura principal de la ménsula según
        el apartado 64.1.2.1.1 de EHE-08.
        '''
        return Fv/self.getCotgStrutAngle()+Fh

    def getAreaNecMainReinforcement(self, Fv,Fh):
        '''getAreaNecMainReinforcement(self, Fv,Fh)

        :param Fv: Vertical load on the corbel, positive downwards (N).
        :param Fh: Horizontal load on the corbel, positive outwards (N).

        Devuelve el área necesaria para la reinforcement principal de 
        la ménsula según el apartado 64.1.2.1.1 de EHE-08.
        '''
        return self.getTraccionMainReinforcement(Fv,Fh)/min(self.steel.fyd(),400e6)
    @staticmethod
    def getTraccionCercos(Fv):
        ''' getTraccionCercos(Fv)

        :param Fv: Vertical load on the corbel, positive downwards (N).

        Devuelve la tracción en los cercos según el apartado 64.1.2.1.1 
        de EHE-08.
        '''
        return 0.2*Fv

    def getAreaNecCercos(self,Fv):
        '''getAreaNecCercos(self,Fv)

        :param Fv: Vertical load on the corbel, positive downwards (N).

        Devuelve el área necesaria para los cercos de la ménsula según
        el apartado 64.1.2.1.1 de EHE-08.
        '''
        return self.getTraccionCercos(Fv)/min(self.steel.fyd(),400e6)

    def getAreaNecApoyo(self,Fv):
        '''getAreaNecApoyo(self,Fv)

        :param Fv: Vertical load on the corbel, positive downwards (N).

        Devuelve el área necesaria para el apoyo según
        el apartado 64.1.2.1.2 de EHE-08.
        '''
        return Fv/0.7/-self.concrete.fcd()



def rasanteAgotamiento(fck,gammac,hf,Asf,Sf,fyd):
    #Cálculo del rasante medio por unidad de longitud que agota la sección
    #del ala de la viga, según el artículo 4.3.2.5 del Eurocódigo 2
    #el resultado está expresado en kN/m (ó N/mm)
    #Datos:
    #fck: resistencia característica a compresión del hormigón (N/mm2)
    #gammac= Partial safety factor for concrete.
    #hf: flange thickness (m)
    #Asf: reinforcement por unidad de longitud que atraviesa la sección (mm2)
    #Sf: spacement of the rebars that cross the section (mm)
    #fyd: resistencia de cálculo de la reinforcement (N/mm2)
    hf=hf*1000     #Flange thickness in mm
    #Esfuerzo rasante de agotamiento por compresión oblicua en la sección
    fcd=fck/gammac
    VRd2=0.2*fcd*hf
    #Esfuerzo rasante de agotamiento por tracción en la sección
    tabla48EC2={12:0.18,16:0.22,20:0.26,25:0.30,30:0.34,35:0.37,40:0.41,45:0.44,50:0.48}
    taoRd=tabla48EC2[fck]
    VRd3=2.5*taoRd*hf+Asf/Sf*fyd
    return min(VRd2,VRd3)

#Ejemplo:
#  esfRasMax=rasanteAgotamiento(25,1.5,300,565,200,500)


# Comprobación de cargas concentradas sobre macizos, según el artículo
# 61 de la EHE-08


# Devuelve el valor de f3cd
def getF3cd(Ac, Ac1, fcd):
  return min(sqrt(Ac/Ac1),3.3)*fcd

def getNuCargaConcentrada(Ac, Ac1, fcd):
  '''
  Devuelve el valor último de la carga que puede soportar una pieza
  de seccion Ac siendo el área cargada Ac1 (ver figura 61.1.a page 302 EHE-08)
  '''
  return Ac1*getF3cd(Ac,Ac1,fcd)

def getUad(a, a1, Nd):
    '''
    Devuelve la capacidad mecánica necesaria para la reinforcement paralela al
    lado a (ver figura 61.1.a page 302 EHE-08)
      a: Dimensión de la sección.
      a1: Dimensión del área cargada paralela a a.
      Nd: Carga concentrada.

    '''
    return 0.25*((a-a1)/a)*Nd

def getAreaArmadAd(a, a1, Nd, fyd):
    '''
    Devuelve el área necesaria para la reinforcement paralela al
    lado a (ver figura 61.1.a page 302 EHE-08)
      a: Dimensión de la sección.
      a1: Dimensión del área cargada paralela a a.
      Nd: Carga concentrada.
      fyd: steel yield strength.

    '''
    return getUad(a,a1,Nd)/min(fyd,400e6)


def getUbd(b, b1, Nd):
    '''
    Devuelve la capacidad mecánica necesaria para la reinforcement paralela al
    lado b (ver figura 61.1.a page 302 EHE-08)
      b: Dimensión de la sección.
      b1: Dimensión del área cargada paralela a b.
      Nd: Carga concentrada.

    '''
    return 0.25*((b-b1)/b)*Nd

def getAreaArmadBd(b, b1, Nd, fyd):
    '''
    Devuelve el área necesaria para la reinforcement paralela al
    lado b (ver figura 61.1.a page 302 EHE-08)
      b: Dimensión de la sección.
      b1: Dimensión del área cargada paralela a a.
      Nd: Carga concentrada.
      fyd: steel yield strength.

    '''
    return getUad(b,b1,Nd)/min(fyd,400e6)
