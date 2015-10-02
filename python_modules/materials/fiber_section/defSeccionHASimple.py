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

class RecordSeccionHASimple(object):
  '''
  This class is used to define the variables that make up a reinforced 
  concrete section with top and bottom reinforcement layers.
  '''
  def __init__(self):
    self.nmbSeccion= "noName"
    self.descSeccion= "Text describing the position of the section in the structure."
    self.tipoHormigon= None
    self.nmbDiagHormigon= None
    self.canto= 0.25
    self.ancho= 0.25
    self.numDivIJ= 10
    self.numDivJK= 10

    # Longitudinal reinforcement
    self.tipoArmadura= None
    self.nmbDiagArmadura= None # Name of the uniaxial material

    self.recubMin= 0.0 

    self.barrasNeg= MainReinfLayer()
    self.barrasPos= MainReinfLayer()

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

  def getConcreteDiagram(self,mdlr):
    return mdlr.getMaterialLoader.getMaterial(self.nmbDiagHormigon)
  def getSteelDiagram(self,mdlr):
    return mdlr.getMaterialLoader.getMaterial(self.nmbDiagArmadura)

  def defSectionGeometry(self,mdlr,tipoDiag):
    '''
    Definición de una sección de hormigón armado sencilla
    con una capa de armadura superior y otra inferior.
    tipoDiag: Cadena de caracteres que será "k" si se emplea el diagrama característico o "d" si se emplea el diagrama de cálculo.
    '''
    if(tipoDiag=="d"):
      if(self.tipoHormigon.tagDiagD<0):
        tagDiagHormigon= self.tipoHormigon.defDiagD(mdlr)
      if(self.tipoArmadura.tagDiagD<0):
        tagDiagAceroArmar= self.tipoArmadura.defDiagD(mdlr)
      self.nmbDiagHormigon= self.tipoHormigon.nmbDiagD
      self.nmbDiagArmadura= self.tipoArmadura.nmbDiagD
    elif(tipoDiag=="k"):
      if(self.tipoHormigon.tagDiagK<0):
        tagDiagHormigon= self.tipoHormigon.defDiagK(mdlr)
      if(self.tipoArmadura.tagDiagK<0):
        tagDiagAceroArmar= self.tipoArmadura.defDiagK(mdlr)
      self.nmbDiagHormigon= self.tipoHormigon.nmbDiagK
      self.nmbDiagArmadura= self.tipoArmadura.nmbDiagK

    geomSection= mdlr.getMaterialLoader.newSectionGeometry(self.nmbGeomSeccion())
    regiones= geomSection.getRegions
    rg= regiones.newQuadRegion(self.nmbDiagHormigon) # Hormigón
    rg.nDivIJ= self.numDivIJ
    rg.nDivJK= self.numDivJK
    rg.pMin= geom.Pos2d(-self.ancho/2,-self.canto/2)
    rg.pMax= geom.Pos2d(self.ancho/2,self.canto/2)
    armaduras= geomSection.getReinfLayers
    if(self.barrasNeg.nBarras>0):
      self.negReinfLayer= armaduras.newStraightReinfLayer(self.nmbDiagArmadura)
      self.negReinfLayer.codigo= "neg"
      self.negReinfLayer.numReinfBars= self.barrasNeg.nBarras
      #print "armadura neg. num. barras: ", self.negReinfLayer.numReinfBars
      self.negReinfLayer.barDiameter= self.barrasNeg.diamBarras
      self.negReinfLayer.barArea= self.barrasNeg.areaBarras
      #print "armadura neg. bar area= ", self.negReinfLayer.barArea*1e6, " mm2"
      #print "armadura neg. bar diam: ", self.barrasNeg.diamBarras*1e3, " mm"
      y= self.getYAsNeg()
      #print "y neg.= ", y, " m"
      self.negReinfLayer.p1= geom.Pos2d(-self.ancho/2+self.barrasNeg.recubLat,y) # Armadura inferior (cara -).
      self.negReinfLayer.p2= geom.Pos2d(self.ancho/2-self.barrasNeg.recubLat,y)

    if(self.barrasPos.nBarras>0):
      self.posReinfLayer= armaduras.newStraightReinfLayer(self.nmbDiagArmadura)
      self.posReinfLayer.codigo= "pos"
      self.posReinfLayer.numReinfBars= self.barrasPos.nBarras
      #print "ancho= ", self.ancho, " m canto= ", self.canto, " m"
      #print "nDivIJ= ", rg.nDivIJ, " nDivJK= ", rg.nDivJK
      #print "armadura pos. num. barras: ", self.posReinfLayer.numReinfBars
      self.posReinfLayer.barDiameter= self.barrasPos.diamBarras
      self.posReinfLayer.barArea= self.barrasPos.areaBarras
      #print "armadura pos. bar area= ", self.posReinfLayer.barArea*1e6, " mm2"
      #print "armadura pos. bar diam: ", self.barrasPos.diamBarras*1e3, " mm"
      y= self.getYAsPos()
      #print "y pos.= ", y, " m"
      self.posReinfLayer.p1= geom.Pos2d(-self.ancho/2+self.barrasPos.recubLat,y) # Armadura superior (cara +).
      self.posReinfLayer.p2= geom.Pos2d(self.ancho/2-self.barrasPos.recubLat,y)

    self.recubMin= min(self.barrasNeg.recubLat,min(self.barrasPos.recubLat,min(self.barrasPos.recub,self.barrasNeg.recub)))

  def getJTorsion(self):
    return parametrosSeccionRectangular.getJTorsion(self.ancho,self.canto)

  def getRespT(self,mdlr,JTorsion):
    '''Material for modeling torsional response of section'''
    return typical_materials.defElasticMaterial(mdlr,self.nmbRespT(),self.tipoHormigon.Gcm()*JTorsion) # Respuesta de la sección a torsión.

  def getRespVy(self,mdlr):
    '''Material for modeling z shear response of section'''
    return typical_materials.defElasticMaterial(mdlr,self.nmbRespVy(),5/6.0*self.ancho*self.canto*self.tipoHormigon.Gcm())

  def getRespVz(self,mdlr):
    '''Material for modeling z shear response of section'''
    return typical_materials.defElasticMaterial(mdlr,self.nmbRespVz(),5/6.0*self.ancho*self.canto*self.tipoHormigon.Gcm())

  def defSeccionHASimple(self, mdlr,tipoDiag):
    '''
    Definición de una sección de hormigón armado sencilla
    con una capa de armadura superior y otra inferior.
    tipoDiag: Tipo del diagrama k: característico, d: cálculo.
    nmbRutinaDefGeom: Nombre de la rutina que define la geometría de la sección.
    '''
    self.JTorsion= self.getJTorsion()
    self.respT= self.getRespT(mdlr,self.JTorsion) # Respuesta de la sección a torsión.
    self.respVy= self.getRespVy(mdlr)
    self.respVz= self.getRespVz(mdlr)

    self.defSectionGeometry(mdlr,tipoDiag)
    fs= mdlr.getMaterialLoader.newMaterial("fiberSectionShear3d",self.nmbSeccion)
    fiberSectionRepr= fs.getFiberSectionRepr()
    fiberSectionRepr.setGeomNamed(self.nmbGeomSeccion())
    fs.setupFibers()

    fs.setRespVyByName(self.nmbRespVy())
    fs.setRespVzByName(self.nmbRespVz())
    fs.setRespTByName(self.nmbRespT())
    fs.setProp("datosSecc",self)
    return fs

  def defInteractionDiagram(self,mdlr,tipoDiag):
    'Defines 3D interaction diagram.'

    self.defSectionGeometry(mdlr,tipoDiag)
    fs= mdlr.getMaterialLoader.newMaterial("fiber_section_3d",self.nmbSeccion)
    fiberSectionRepr= fs.getFiberSectionRepr()
    fiberSectionRepr.setGeomNamed(self.nmbGeomSeccion())
    fs.setupFibers()

    param= xc.InteractionDiagramParameters()
    if(tipoDiag=="d"):
      param.tagHormigon= self.tipoHormigon.tagDiagD
      param.tagArmadura= self.tipoArmadura.tagDiagD
    elif(tipoDiag=="k"):
      param.tagHormigon= self.tipoHormigon.tagDiagK
      param.tagArmadura= self.tipoArmadura.tagDiagK
    return mdlr.getMaterialLoader.calcInteractionDiagram(self.nmbSeccion,param)

  def defInteractionDiagramNMy(self,mdlr,tipoDiag):
    'Defines N-My interaction diagram.'

    self.defSectionGeometry(mdlr,tipoDiag)
    fs= mdlr.getMaterialLoader.newMaterial("fiber_section_3d",self.nmbSeccion)
    fiberSectionRepr= fs.getFiberSectionRepr()
    fiberSectionRepr.setGeomNamed(self.nmbGeomSeccion())
    fs.setupFibers()

    param= xc.InteractionDiagramParameters()
    if(tipoDiag=="d"):
      param.tagHormigon= self.tipoHormigon.tagDiagD
      param.tagArmadura= self.tipoArmadura.tagDiagD
    elif(tipoDiag=="k"):
      param.tagHormigon= self.tipoHormigon.tagDiagK
      param.tagArmadura= self.tipoArmadura.tagDiagK
    return mdlr.getMaterialLoader.calcInteractionDiagramNMy(self.nmbSeccion,param)

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
