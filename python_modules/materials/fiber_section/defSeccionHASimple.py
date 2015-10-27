# -*- coding: utf-8 -*-

'''
Definición de una sección de hormigón armado sencilla
con una sola capa de armadura.
'''
from __future__ import division
import xc_base
import geom
import xc
from materials import parametrosSeccionRectangular
from materials import typical_materials
import math
from materials.ehe import areaBarrasEHE
from materials import stressCalc as sc
import sys

class RecordArmaduraCortante(object):
  # Definition of the variables that make up a family of shear reinforcing bars

  def __init__(self):
    self.nmbFamilia= "noName" # name identifying the family of shear reinforcing bars
    self.nRamas= 0.0 # Number of effective branches
    self.areaRama= 0.0 # Area of the shear reinforcing bar
    self.espaciamientoRamas= 0.2 # longitudinal distance between transverse reinforcements
    self.angAlphaRamas= math.pi/2.0 # angle between the shear reinforcing bars and the axis of the member.
    self.angThetaBielas= math.pi/4.0 # angle between the concrete's compression struts and the axis of the member
    
  def getAs(self):
    'returns the area per unit length of the family of shear reinforcements'
    return self.nRamas*self.areaRama/self.espaciamientoRamas


class MainReinfLayer(object):
  def __init__(self,diam=10e-3,area= areaBarrasEHE.Fi10,spacing=0.2,ancho=1.0,basicCover=0.03):
    self.diamBarras= diam
    self.sepBarras= spacing
    nBarrasTeor= ancho/self.sepBarras
    self.nBarras= int(math.floor(nBarrasTeor))
    self.areaBarras= area
    self.recub= basicCover+self.diamBarras/2.0
    self.centraBarras(ancho)
  def setUp(self,nBarras= 5, diam=10e-3,area= areaBarrasEHE.Fi10,ancho=1.0,recub=0.03):
    self.nBarras= nBarras
    self.diamBarras= diam
    if(self.nBarras!=0.0):
      self.sepBarras= ancho/self.nBarras
      self.centraBarras(ancho)
    else:
      self.sepBarras= 100.0
    self.areaBarras= area
    self.recub= recub
    
  def getAs(self):
    '''Steel area.'''
    return self.nBarras*self.areaBarras
  def centraBarras(self,ancho):
    self.recubLat= (ancho-(self.nBarras-1)*self.sepBarras)/2.0

  def defReinfLayer(self,armaduras,code,nmbDiagram,p1,p2):
    '''Definition of a reinforcement layer in the fiber section model.'''
    if(self.nBarras>0):
      self.reinfLayer= armaduras.newStraightReinfLayer(nmbDiagram)
      self.reinfLayer.codigo= code
      self.reinfLayer.numReinfBars= self.nBarras
      #print "armadura ", cod, " num. barras: ", self.reinfLayer.numReinfBars
      self.reinfLayer.barDiameter= self.diamBarras
      self.reinfLayer.barArea= self.areaBarras
      #print "armadura", cod, " bar area= ", self.reinfLayer.barArea*1e6, " mm2"
      #print "armadura", cod, " bar diam: ", self.diamBarras*1e3, " mm"
      self.reinfLayer.p1= p1
      self.reinfLayer.p2= p2
      return self.reinfLayer

class BasicRecordRCSection(object):
  '''
  This class is used to define the basic variables that make up a reinforced 
  concrete section.
  '''
  def __init__(self):
    self.nmbSeccion= "noName"
    self.descSeccion= "Text describing the position of the section in the structure."
    self.tipoHormigon= None
    self.nmbDiagHormigon= None
    self.canto= 0.25
    self.ancho= 0.25
    self.nDivIJ= 10
    self.nDivJK= 10
    self.fiberSectionRepr= None

    self.tipoArmadura= None
    self.nmbDiagArmadura= None # Name of the uniaxial material

    # Transverse reinforcement (z direction)
    self.armCortanteZ= RecordArmaduraCortante()
    self.armCortanteZ.nmbFamilia= "Vz"

    # Transverse reinforcement (y direction)
    self.armCortanteY= RecordArmaduraCortante()
    self.armCortanteY.nmbFamilia= "Vy"

  def nmbGeomSeccion(self):
    return "geom"+self.nmbSeccion
  def nmbRespT(self):
    return self.nmbSeccion+"RespT"
  def nmbRespVy(self):
    return self.nmbSeccion+"RespVy"
  def nmbRespVz(self):
    return self.nmbSeccion+"RespVz"

  def getConcreteDiagram(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.nmbDiagHormigon)
  def getSteelDiagram(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.nmbDiagArmadura)
  def getSteelEquivalenceCoefficient(self,preprocessor):
    tangHorm= self.getConcreteDiagram(preprocessor).getTangent()
    tangSteel= self.getSteelDiagram(preprocessor).getTangent()
    return tangSteel/tangHorm

  def defDiagrams(self,preprocessor,tipoDiag):
    '''
    Stress-strain diagrams definition.
    '''
    self.diagType= tipoDiag
    if(self.diagType=="d"):
      if(self.tipoHormigon.tagDiagD<0):
        tagDiagHormigon= self.tipoHormigon.defDiagD(preprocessor)
      if(self.tipoArmadura.tagDiagD<0):
        tagDiagAceroArmar= self.tipoArmadura.defDiagD(preprocessor)
      self.nmbDiagHormigon= self.tipoHormigon.nmbDiagD
      self.nmbDiagArmadura= self.tipoArmadura.nmbDiagD
    elif(self.diagType=="k"):
      if(self.tipoHormigon.tagDiagK<0):
        tagDiagHormigon= self.tipoHormigon.defDiagK(preprocessor)
      if(self.tipoArmadura.tagDiagK<0):
        tagDiagAceroArmar= self.tipoArmadura.defDiagK(preprocessor)
      self.nmbDiagHormigon= self.tipoHormigon.nmbDiagK
      self.nmbDiagArmadura= self.tipoArmadura.nmbDiagK

  def defConcreteRegion(self,geomSection):
    regiones= geomSection.getRegions
    rg= regiones.newQuadRegion(self.nmbDiagHormigon) # Hormigón
    rg.nDivIJ= self.nDivIJ
    rg.nDivJK= self.nDivJK
    rg.pMin= geom.Pos2d(-self.ancho/2,-self.canto/2)
    rg.pMax= geom.Pos2d(self.ancho/2,self.canto/2)


class RecordSeccionHASimple(BasicRecordRCSection):
  '''
  This class is used to define the variables that make up a reinforced 
  concrete section with top and bottom reinforcement layers.
  '''
  def __init__(self):
    super(RecordSeccionHASimple,self).__init__()

    # Longitudinal reinforcement

    self.recubMin= 0.0 

    self.barrasNeg= MainReinfLayer()
    self.barrasPos= MainReinfLayer()


  def getAsPos(self):
    return self.barrasPos.getAs()
  def getYAsPos(self):
    return self.canto/2.0-self.barrasPos.recub
  def getAsNeg(self):
    return self.barrasNeg.getAs()
  def getYAsNeg(self):
    return -self.canto/2.0+self.barrasNeg.recub
  def getAc(self):
    return self.ancho*self.canto
  def getI(self):
    return 1/12.0*self.ancho*self.canto**3

  def getSNeg(self):
    '''distance between bars in local negative face.'''
    return self.barrasNeg.sepBarras
  def getSPos(self):
    '''distance between bars in local positive face.'''
    return self.barrasPos.sepBarras
  def getDiamNeg(self):
    '''bar diameter in local negative face.'''
    return self.barrasNeg.diamBarras
  def getDiamPos(self):
    '''bar diameter in local positive face.'''
    return self.barrasPos.diamBarras
  def getNBarNeg(self):
    '''number of bars in local negative face.'''
    return self.barrasNeg.nBarras
  def getNBarPos(self):
    '''number of bars in local positive face.'''
    return self.barrasPos.nBarras

  def centraBarrasPos(self):
    self.barrasPos.centraBarras(self.ancho)
  def centraBarrasNeg(self):
    self.barrasNeg.centraBarras(self.ancho)

  def setMainReinfNeg(self,diam,area,spacing,basicCover):
    self.barrasNeg= MainReinfLayer(diam,area,spacing,self.ancho,basicCover)
  def setMainReinfPos(self,diam,area,spacing,basicCover):
    self.barrasPos= MainReinfLayer(diam,area,spacing,self.ancho,basicCover)


  def defSectionGeometry(self,preprocessor,tipoDiag):
    '''
    Definición de una sección de hormigón armado sencilla
    con una capa de armadura superior y otra inferior.
    tipoDiag: Cadena de caracteres que será "k" si se emplea el diagrama característico o "d" si se emplea el diagrama de cálculo.
    '''
    self.defDiagrams(preprocessor,tipoDiag)

    geomSection= preprocessor.getMaterialLoader.newSectionGeometry(self.nmbGeomSeccion())
    self.defConcreteRegion(geomSection)

    armaduras= geomSection.getReinfLayers
    y= self.getYAsNeg()
    #print "y neg.= ", y, " m"
    p1= geom.Pos2d(-self.ancho/2+self.barrasNeg.recubLat,y) # Armadura inferior (cara -).
    p2= geom.Pos2d(self.ancho/2-self.barrasNeg.recubLat,y)
    self.negReinfLayer= self.barrasNeg.defReinfLayer(armaduras,"neg",self.nmbDiagArmadura,p1,p2)

    y= self.getYAsPos()
    p1= geom.Pos2d(-self.ancho/2+self.barrasPos.recubLat,y) # Armadura superior (cara +).
    p2= geom.Pos2d(self.ancho/2-self.barrasPos.recubLat,y)
    self.posReinfLayer= self.barrasPos.defReinfLayer(armaduras,"pos",self.nmbDiagArmadura,p1,p2)

    self.recubMin= min(self.barrasNeg.recubLat,min(self.barrasPos.recubLat,min(self.barrasPos.recub,self.barrasNeg.recub)))

  def getJTorsion(self):
    return parametrosSeccionRectangular.getJTorsion(self.ancho,self.canto)

  def getRespT(self,preprocessor,JTorsion):
    '''Material for modeling torsional response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.nmbRespT(),self.tipoHormigon.Gcm()*JTorsion) # Respuesta de la sección a torsión.

  def getRespVy(self,preprocessor):
    '''Material for modeling z shear response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.nmbRespVy(),5/6.0*self.ancho*self.canto*self.tipoHormigon.Gcm())

  def getRespVz(self,preprocessor):
    '''Material for modeling z shear response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.nmbRespVz(),5/6.0*self.ancho*self.canto*self.tipoHormigon.Gcm())

  def defFiberSection(self,preprocessor):
    self.fs= preprocessor.getMaterialLoader.newMaterial("fiberSectionShear3d",self.nmbSeccion)
    self.fiberSectionRepr= self.fs.getFiberSectionRepr()
    self.fiberSectionRepr.setGeomNamed(self.nmbGeomSeccion())
    self.fs.setupFibers()

    self.fs.setRespVyByName(self.nmbRespVy())
    self.fs.setRespVzByName(self.nmbRespVz())
    self.fs.setRespTByName(self.nmbRespT())
    self.fs.setProp("datosSecc",self)

  def defSeccionHASimple(self, preprocessor,tipoDiag):
    '''
    Definición de una sección de hormigón armado sencilla
    con una capa de armadura superior y otra inferior.
    tipoDiag: Tipo del diagrama k: característico, d: cálculo.
    nmbRutinaDefGeom: Nombre de la rutina que define la geometría de la sección.
    '''
    self.JTorsion= self.getJTorsion()
    self.respT= self.getRespT(preprocessor,self.JTorsion) # Respuesta de la sección a torsión.
    self.respVy= self.getRespVy(preprocessor)
    self.respVz= self.getRespVz(preprocessor)

    self.defSectionGeometry(preprocessor,tipoDiag)
    self.defFiberSection(preprocessor)

  def defInteractionDiagramParameters(self, preprocessor):
    ''' parameters for interaction diagrams. '''
    self.param= xc.InteractionDiagramParameters()
    if(self.diagType=="d"):
      self.param.tagHormigon= self.tipoHormigon.tagDiagD
      self.param.tagArmadura= self.tipoArmadura.tagDiagD
    elif(self.diagType=="k"):
      self.param.tagHormigon= self.tipoHormigon.tagDiagK
      self.param.tagArmadura= self.tipoArmadura.tagDiagK
    return self.param

  def defInteractionDiagram(self,preprocessor):
    'Defines 3D interaction diagram.'
    if(not self.fiberSectionRepr):
      sys.stderr.write("defInteractionDiagram: fiber section representation for section: "+ self.nmbSeccion + ";  not defined use defFiberSection.\n")
    self.defInteractionDiagramParameters(preprocessor)
    return preprocessor.getMaterialLoader.calcInteractionDiagram(self.nmbSeccion,self.param)

  def defInteractionDiagramNMy(self,preprocessor):
    'Defines N-My interaction diagram.'
    if(not self.fiberSectionRepr):
      sys.stderr.write("defInteractionDiagramNMy: fiber section representation for section: "+ self.nmbSeccion + ";  not defined use defFiberSection.\n")
    self.defInteractionDiagramParameters(preprocessor)
    return preprocessor.getMaterialLoader.calcInteractionDiagramNMy(self.nmbSeccion,self.param)

  def defInteractionDiagramNMz(self,preprocessor):
    'Defines N-My interaction diagram.'
    if(not self.fiberSectionRepr):
      sys.stderr.write("defInteractionDiagramNMz: fiber section representation for section: "+ self.nmbSeccion + ";  not defined use defFiberSection.\n")
    self.defInteractionDiagramParameters(preprocessor)
    return preprocessor.getMaterialLoader.calcInteractionDiagramNMz(self.nmbSeccion,self.param)

  def getStressCalculator(self):
    Ec= self.tipoHormigon.Ecm()
    Es= self.tipoArmadura.Es
    return sc.StressCalc(self.ancho,self.canto,self.barrasPos.recub,self.barrasNeg.recub,self.getAsPos(),self.getAsNeg(),Ec,Es)

class RecordSeccionHALosa(object):
  '''
  Define las variables que se emplean para definir
     las secciones de hormigón armado de una losa sencilla con una
     capa de armadura superior y otra inferior.
  '''
  def __init__(self,nmb,desc,canto,concrete,steel,basicCover):
    self.name= nmb
    self.basicCover= basicCover
    self.D2Section= RecordSeccionHASimple()
    self.D2Section.nmbSeccion= nmb + "2"
    self.D2Section.descSeccion= desc + ". 2 direction."
    self.D2Section.tipoHormigon= concrete
    self.D2Section.canto= canto
    self.D2Section.ancho= 1.0
    self.D2Section.tipoArmadura= steel

    self.D1Section= RecordSeccionHASimple()
    self.D1Section.nmbSeccion= nmb + "1"
    self.D1Section.descSeccion= desc + ". 1 direction."
    self.D1Section.tipoHormigon= concrete
    self.D1Section.canto= canto
    self.D1Section.ancho= 1.0
    self.D1Section.tipoArmadura= steel

  def setMainReinf2neg(self,diam,area,spacing):
    self.D2Section.setMainReinfNeg(diam,area,spacing,self.basicCover)

  def setMainReinf2pos(self,diam,area,spacing):
    self.D2Section.setMainReinfPos(diam,area,spacing,self.basicCover)

  def setShearReinfD2(self,nRamas,areaRama,spacing):
    self.D2Section.armCortanteZ.nRamas= nRamas # Número de ramas eficaces frente al cortante.
    self.D2Section.armCortanteZ.areaRama= areaRama # Área de cada barra.
    self.D2Section.armCortanteZ.espaciamientoRamas= spacing

  def setShearReinfD1(self,nRamas,areaRama,spacing):
    self.D1Section.armCortanteZ.nRamas= nRamas # Número de ramas eficaces frente al cortante.
    self.D1Section.armCortanteZ.areaRama= areaRama # Área de cada barra.
    self.D1Section.armCortanteZ.espaciamientoRamas= spacing

  def setMainReinf1neg(self,diam,area,spacing):
    self.D1Section.setMainReinfNeg(diam,area,spacing,self.basicCover+self.D2Section.barrasNeg.diamBarras)

  def setMainReinf1pos(self,diam,area,spacing):
    self.D1Section.setMainReinfPos(diam,area,spacing,self.basicCover+self.D2Section.barrasPos.diamBarras)

  def getAs1neg(self):
    '''Steel area in local negative face direction 1.'''
    return self.D1Section.getAsNeg()
  def getAs1pos(self):
    '''Steel area in local positive face direction 1.'''
    return self.D1Section.getAsPos()
  def getAs2neg(self):
    '''Steel area in local negative face direction 2.'''
    return self.D2Section.getAsNeg()
  def getAs2pos(self):
    '''Steel area in local positive face direction 2.'''
    return self.D2Section.getAsPos()
  def getReinfArea(self,code):
    '''get steel area.'''
    if(code=='As1-'):
      return self.getAs1neg()
    elif(code=='As1+'):
      return self.getAs1pos()
    elif(code=='As2-'):
      return self.getAs2neg()
    elif(code=='As2+'):
      return self.getAs2pos()
    else:
      sys.stderr.write("code: "+ code + " unknown.\n")
      return None

  def getS1neg(self):
    '''distance between bars in local negative face direction 1.'''
    return self.D1Section.getSNeg()
  def getS1pos(self):
    '''distance between bars in local positive face direction 1.'''
    return self.D1Section.getSPos()
  def getS2neg(self):
    '''distance between bars in local negative face direction 2.'''
    return self.D2Section.getSNeg()
  def getS2pos(self):
    '''distance between bars in local positive face direction 2.'''
    return self.D2Section.getSPos()
  def getS(self,code):
    '''distance between bars.'''
    if(code=='s1-'):
      return self.getS1neg()
    elif(code=='s1+'):
      return self.getS1pos()
    elif(code=='s2-'):
      return self.getS2neg()
    elif(code=='s2+'):
      return self.getS2pos()
    else:
      sys.stderr.write("code: "+ code + " unknown.\n")
      return None

  def getDiam1neg(self):
    '''bar diameter in local negative face direction 1.'''
    return self.D1Section.getDiamNeg()
  def getDiam1pos(self):
    '''bar diameter in local positive face direction 1.'''
    return self.D1Section.getDiamPos()
  def getDiam2neg(self):
    '''bar diameter in local negative face direction 2.'''
    return self.D2Section.getDiamNeg()
  def getDiam2pos(self):
    '''bar diameter in local positive face direction 2.'''
    return self.D2Section.getDiamPos()
  def getDiam(self,code):
    '''bar diameter.'''
    if(code=='d1-'):
      return self.getDiam1neg()
    elif(code=='d1+'):
      return self.getDiam1pos()
    elif(code=='d2-'):
      return self.getDiam2neg()
    elif(code=='d2+'):
      return self.getDiam2pos()
    else:
      sys.stderr.write("code: "+ code + " unknown.\n")
      return None

  def getNBar1neg(self):
    '''number of bars in local negative face direction 1.'''
    return self.D1Section.getNBarNeg()
  def getNBar1pos(self):
    '''number of bars in local positive face direction 1.'''
    return self.D1Section.getNBarPos()
  def getNBar2neg(self):
    '''number of bars in local negative face direction 2.'''
    return self.D2Section.getNBarNeg()
  def getNBar2pos(self):
    '''number of bars in local positive face direction 2.'''
    return self.D2Section.getNBarPos()
  def getNBar(self,code):
    '''number of bars.'''
    if(code=='nBars1-'):
      return self.getNBar1neg()
    elif(code=='nBars1+'):
      return self.getNBar1pos()
    elif(code=='nBars2-'):
      return self.getNBar2neg()
    elif(code=='nBars2+'):
      return self.getNBar2pos()
    else:
      sys.stderr.write("getNBar; code: "+ code + " unknown.\n")
      return None

  def getMainReinfProperty(self,code):
    if('As' in code):
      return self.getReinfArea(code)
    elif('nBar' in code):
      return self.getNBar(code)
    elif('s' in code):
      return self.getS(code)
    elif('d' in code):
      return self.getDiam(code)


def loadMainRefPropertyIntoElements(elemSet, sectionContainer, code):
  '''add to each element of the set the
     desired property (As1+,As1-,...,d1+,d1-,...).''' 
  for e in elemSet:
    if(e.hasProp('sectionName')):
      sectionName= e.getProp('sectionName')
      s= sectionContainer.search(sectionName)
      e.setProp(code,s.getMainReinfProperty(code))
    else:
      sys.stderr.write("element: "+ str(e.tag) + " section undefined.\n")
      e.setProp(code,0.0)
