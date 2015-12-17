# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import xc_base
import geom
import xc
from materials import parametrosSeccionRectangular
from materials import typical_materials
import math
from materials.ehe import areaBarrasEHE
from materials import stressCalc as sc
import sys

class RecordShearReinforcement(object):
  ''' Definition of the variables that make up a family of shear 
  reinforcing bars
  Attributes:
    familyName:        name identifying the family of shear reinforcing bars
    nShReinfBranches:  number of effective branches 
    shReinfSpacing:    longitudinal distance between transverse reinforcements
    angAlphaShReinf:   angle between the shear reinforcing bars and the axis 
                       of the member
    angThetaConcrStruts: angle between the concrete's compression struts 
                         and the axis of the member
  '''
  def __init__(self):
    self.familyName= "noName" # name identifying the family of shear reinforcing bars
    self.nShReinfBranches= 0.0 # Number of effective branches
    self.areaShReinfBranch= 0.0 # Area of the shear reinforcing bar
    self.shReinfSpacing= 0.2 # longitudinal distance between transverse reinforcements
    self.angAlphaShReinf= math.pi/2.0 # angle between the shear reinforcing bars and the axis of the member.
    self.angThetaConcrStruts= math.pi/4.0 # angle between the concrete's compression struts and the axis of the member
    
  def getAs(self):
    '''returns the area per unit length of the family of shear reinforcements
    '''
    return self.nShReinfBranches*self.areaShReinfBranch/self.shReinfSpacing


class MainReinfLayer(object):
  ''' Definition of the variables that make up a family (row) of main 
  (longitudinal) reinforcing bars
  Attributes:
    rebarsDiam:    diameter of the bars
    rebarsSpacing: spacing between bars
    nRebars:       number of rebars to be placed in the row
    areaRebar:     cross-sectional area of the bar
    cover:         effective cover
  '''
  def __init__(self,diam=10e-3,area= areaBarrasEHE.Fi10,spacing=0.2,width=1.0,basicCover=0.03):
    self.rebarsDiam= diam
    self.rebarsSpacing= spacing
    nRebarsTeor= width/self.rebarsSpacing
    self.nRebars= int(math.floor(nRebarsTeor))
    self.areaRebar= area
    self.cover= basicCover+self.rebarsDiam/2.0
    self.centerRebars(width)
  def setUp(self,nRebars= 5, diam=10e-3,area= areaBarrasEHE.Fi10,width=1.0,cover=0.03):
    self.nRebars= nRebars
    self.rebarsDiam= diam
    if(self.nRebars!=0.0):
      self.rebarsSpacing= width/self.nRebars
      self.centerRebars(width)
    else:
      self.rebarsSpacing= 100.0
    self.areaRebar= area
    self.cover= cover
    
  def getAs(self):
    '''returns the total cross-sectional area of reinforcing steel in the family
    '''
    return self.nRebars*self.areaRebar
  def centerRebars(self,width):
    '''center the row of rebars in the width of the section'''
    self.coverLat= (width-(self.nRebars-1)*self.rebarsSpacing)/2.0

  def defReinfLayer(self,armaduras,code,nmbDiagram,p1,p2):
    '''Definition of a reinforcement layer in the fiber section model.'''
    if(self.nRebars>0):
      self.reinfLayer= armaduras.newStraightReinfLayer(nmbDiagram)
      self.reinfLayer.codigo= code
      self.reinfLayer.numReinfBars= self.nRebars
      #print "armadura ", cod, " num. barras: ", self.reinfLayer.numReinfBars
      self.reinfLayer.barDiameter= self.rebarsDiam
      self.reinfLayer.barArea= self.areaRebar
      #print "armadura", cod, " bar area= ", self.reinfLayer.barArea*1e6, " mm2"
      #print "armadura", cod, " bar diam: ", self.rebarsDiam*1e3, " mm"
      self.reinfLayer.p1= p1
      self.reinfLayer.p2= p2
      return self.reinfLayer

class BasicRecordRCSection(object):
  '''
  This class is used to define the basic variables that make up a reinforced 
  concrete section.
  Attributes:
    sectionName:     name identifying the section
    sectionDescr:    section description
    concrType:       type of concrete (e.g. hormigonesEHE.HA25)     
    concrDiagName:   name identifying the characteristic stress-strain diagram of the concrete material
    depth:           cross-section depth
    width:           cross-section width
    nDivIJ:          number of cells in IJ (width) direction
    nDivJK:          number of cells in JK  (height) direction
    fiberSectionRepr:
    reinfSteelType:  type of reinforcement steel
    reinfDiagName:   name identifying the characteristic stress-strain diagram of the reinforcing steel material
    shReinfZ:        record of type defSeccionHASimple.RecordShearReinforcement() defining the shear reinforcement in Z direction
    shReinfY:        record of type defSeccionHASimple.RecordShearReinforcement() defining the shear reinforcement in Y direction
  '''
  def __init__(self):
    self.sectionName= "noName"
    self.sectionDescr= "Text describing the position of the section in the structure."
    self.concrType= None
    self.concrDiagName= None
    self.depth= 0.25
    self.width= 0.25
    self.nDivIJ= 10
    self.nDivJK= 10
    self.fiberSectionRepr= None

    self.reinfSteelType= None
    self.reinfDiagName= None # Name of the uniaxial material

    # Transverse reinforcement (z direction)
    self.shReinfZ= RecordShearReinforcement()
    self.shReinfZ.familyName= "Vz"

    # Transverse reinforcement (y direction)
    self.shReinfY= RecordShearReinforcement()
    self.shReinfY.familyName= "Vy"

  def gmSectionName(self):
    ''' returns de name of the geometric section'''
    return "geom"+self.sectionName
  def nmbRespT(self):
    return self.sectionName+"RespT"
  def nmbRespVy(self):
    return self.sectionName+"RespVy"
  def nmbRespVz(self):
    return self.sectionName+"RespVz"

  def getConcreteDiagram(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.concrDiagName)
  def getSteelDiagram(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.reinfDiagName)
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
      if(self.concrType.matTagD<0):
        matTagHormigon= self.concrType.defDiagD(preprocessor)
      if(self.reinfSteelType.matTagD<0):
        matTagAceroArmar= self.reinfSteelType.defDiagD(preprocessor)
      self.concrDiagName= self.concrType.nmbDiagD
      self.reinfDiagName= self.reinfSteelType.nmbDiagD
    elif(self.diagType=="k"):
      if(self.concrType.matTagK<0):
        matTagHormigon= self.concrType.defDiagK(preprocessor)
      if(self.reinfSteelType.matTagK<0):
        matTagAceroArmar= self.reinfSteelType.defDiagK(preprocessor)
      self.concrDiagName= self.concrType.nmbDiagK
      self.reinfDiagName= self.reinfSteelType.nmbDiagK

  def defConcreteRegion(self,geomSection):
    regiones= geomSection.getRegions
    rg= regiones.newQuadRegion(self.concrDiagName) 
    rg.nDivIJ= self.nDivIJ
    rg.nDivJK= self.nDivJK
    rg.pMin= geom.Pos2d(-self.width/2,-self.depth/2)
    rg.pMax= geom.Pos2d(self.width/2,self.depth/2)


class RecordSeccionHASimple(BasicRecordRCSection):
  '''
  This class is used to define the variables that make up a reinforced 
  concrete section with top and bottom reinforcement layers.
  Attributes:
    sectionName:     name identifying the section
    sectionDescr:    section description
    concrType:       type of concrete (e.g. hormigonesEHE.HA25)     
    concrDiagName:   name identifying the characteristic stress-strain diagram of the concrete material
    depth:           cross-section depth
    width:           cross-section width
    nDivIJ:          number of cells in IJ (width) direction
    nDivJK:          number of cells in JK  (height) direction
    fiberSectionRepr:
    reinfSteelType:  type of reinforcement steel
    reinfDiagName:   name identifying the characteristic stress-strain diagram of
                     the reinforcing steel material
    shReinfZ:        record of type defSeccionHASimple.RecordShearReinforcement()
                     defining the shear reinforcement in Z direction
    shReinfY:        record of type defSeccionHASimple.RecordShearReinforcement() 
                     defining the shear reinforcement in Y direction
    coverMin:        minimum value of end or clear concrete cover of main bars from
                     both the positive and negative faces
    negRebars:       layer of main rebars in the local negative face of the section
    posRebars:       layer of main rebars in the local positive face of the section
  '''
  def __init__(self):
    super(RecordSeccionHASimple,self).__init__()

    # Longitudinal reinforcement
    self.coverMin= 0.0 
    self.negRebars= MainReinfLayer()
    self.posRebars= MainReinfLayer()

  def getAsPos(self):
    return self.posRebars.getAs()
  def getYAsPos(self):
    return self.depth/2.0-self.posRebars.cover
  def getAsNeg(self):
    return self.negRebars.getAs()
  def getYAsNeg(self):
    return -self.depth/2.0+self.negRebars.cover
  def getAc(self):
    return self.width*self.depth
  def getI(self):
    return 1/12.0*self.width*self.depth**3

  def getSNeg(self):
    '''distance between bars in local negative face.'''
    return self.negRebars.rebarsSpacing
  def getSPos(self):
    '''distance between bars in local positive face.'''
    return self.posRebars.rebarsSpacing
  def getDiamNeg(self):
    '''bar diameter in local negative face.'''
    return self.negRebars.rebarsDiam
  def getDiamPos(self):
    '''bar diameter in local positive face.'''
    return self.posRebars.rebarsDiam
  def getNBarNeg(self):
    '''number of bars in local negative face.'''
    return self.negRebars.nRebars
  def getNBarPos(self):
    '''number of bars in local positive face.'''
    return self.posRebars.nRebars

  def centerRebarsPos(self):
    self.posRebars.centerRebars(self.width)
  def centerRebarsNeg(self):
    self.negRebars.centerRebars(self.width)

  def setMainReinfNeg(self,diam,area,spacing,basicCover):
    self.negRebars= MainReinfLayer(diam,area,spacing,self.width,basicCover)
  def setMainReinfPos(self,diam,area,spacing,basicCover):
    self.posRebars= MainReinfLayer(diam,area,spacing,self.width,basicCover)


  def defSectionGeometry(self,preprocessor,tipoDiag):
    '''
    Definición de una sección de hormigón armado sencilla
    con una capa de armadura superior y otra inferior.
    tipoDiag: Cadena de caracteres que será "k" si se emplea el diagrama característico o "d" si se emplea el diagrama de cálculo.
    '''
    self.defDiagrams(preprocessor,tipoDiag)

    geomSection= preprocessor.getMaterialLoader.newSectionGeometry(self.gmSectionName())
    self.defConcreteRegion(geomSection)

    armaduras= geomSection.getReinfLayers
    y= self.getYAsNeg()
    #print "y neg.= ", y, " m"
    p1= geom.Pos2d(-self.width/2+self.negRebars.coverLat,y) # Armadura inferior (cara -).
    p2= geom.Pos2d(self.width/2-self.negRebars.coverLat,y)
    self.negReinfLayer= self.negRebars.defReinfLayer(armaduras,"neg",self.reinfDiagName,p1,p2)

    y= self.getYAsPos()
    p1= geom.Pos2d(-self.width/2+self.posRebars.coverLat,y) # Armadura superior (cara +).
    p2= geom.Pos2d(self.width/2-self.posRebars.coverLat,y)
    self.posReinfLayer= self.posRebars.defReinfLayer(armaduras,"pos",self.reinfDiagName,p1,p2)

    self.coverMin= min(self.negRebars.coverLat,min(self.posRebars.coverLat,min(self.posRebars.cover,self.negRebars.cover)))

  def getJTorsion(self):
    return parametrosSeccionRectangular.getJTorsion(self.width,self.depth)

  def getRespT(self,preprocessor,JTorsion):
    '''Material for modeling torsional response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.nmbRespT(),self.concrType.Gcm()*JTorsion) # Respuesta de la sección a torsión.

  def getRespVy(self,preprocessor):
    '''Material for modeling z shear response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.nmbRespVy(),5/6.0*self.width*self.depth*self.concrType.Gcm())

  def getRespVz(self,preprocessor):
    '''Material for modeling z shear response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.nmbRespVz(),5/6.0*self.width*self.depth*self.concrType.Gcm())

  def defFiberSection(self,preprocessor):
    self.fs= preprocessor.getMaterialLoader.newMaterial("fiberSectionShear3d",self.sectionName)
    self.fiberSectionRepr= self.fs.getFiberSectionRepr()
    self.fiberSectionRepr.setGeomNamed(self.gmSectionName())
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
      self.param.tagHormigon= self.concrType.matTagD
      self.param.tagArmadura= self.reinfSteelType.matTagD
    elif(self.diagType=="k"):
      self.param.tagHormigon= self.concrType.matTagK
      self.param.tagArmadura= self.reinfSteelType.matTagK
    return self.param

  def defInteractionDiagram(self,preprocessor):
    'Defines 3D interaction diagram.'
    if(not self.fiberSectionRepr):
      sys.stderr.write("defInteractionDiagram: fiber section representation for section: "+ self.sectionName + ";  not defined use defFiberSection.\n")
    self.defInteractionDiagramParameters(preprocessor)
    return preprocessor.getMaterialLoader.calcInteractionDiagram(self.sectionName,self.param)

  def defInteractionDiagramNMy(self,preprocessor):
    'Defines N-My interaction diagram.'
    if(not self.fiberSectionRepr):
      sys.stderr.write("defInteractionDiagramNMy: fiber section representation for section: "+ self.sectionName + ";  not defined use defFiberSection.\n")
    self.defInteractionDiagramParameters(preprocessor)
    return preprocessor.getMaterialLoader.calcInteractionDiagramNMy(self.sectionName,self.param)

  def defInteractionDiagramNMz(self,preprocessor):
    'Defines N-My interaction diagram.'
    if(not self.fiberSectionRepr):
      sys.stderr.write("defInteractionDiagramNMz: fiber section representation for section: "+ self.sectionName + ";  not defined use defFiberSection.\n")
    self.defInteractionDiagramParameters(preprocessor)
    return preprocessor.getMaterialLoader.calcInteractionDiagramNMz(self.sectionName,self.param)

  def getStressCalculator(self):
    Ec= self.concrType.Ecm()
    Es= self.reinfSteelType.Es
    return sc.StressCalc(self.width,self.depth,self.posRebars.cover,self.negRebars.cover,self.getAsPos(),self.getAsNeg(),Ec,Es)

class RecordSeccionHALosa(object):
  '''
  Define las variables que se emplean para definir
     las secciones de hormigón armado de una losa sencilla con una
     capa de armadura superior y otra inferior.
  '''
  def __init__(self,nmb,desc,depth,concrete,steel,basicCover):
    self.name= nmb
    self.basicCover= basicCover
    self.D2Section= RecordSeccionHASimple()
    self.D2Section.sectionName= nmb + "2"
    self.D2Section.sectionDescr= desc + ". 2 direction."
    self.D2Section.concrType= concrete
    self.D2Section.depth= depth
    self.D2Section.width= 1.0
    self.D2Section.reinfSteelType= steel

    self.D1Section= RecordSeccionHASimple()
    self.D1Section.sectionName= nmb + "1"
    self.D1Section.sectionDescr= desc + ". 1 direction."
    self.D1Section.concrType= concrete
    self.D1Section.depth= depth
    self.D1Section.width= 1.0
    self.D1Section.reinfSteelType= steel

  def setMainReinf2neg(self,diam,area,spacing):
    self.D2Section.setMainReinfNeg(diam,area,spacing,self.basicCover)

  def setMainReinf2pos(self,diam,area,spacing):
    self.D2Section.setMainReinfPos(diam,area,spacing,self.basicCover)

  def setShearReinfD2(self,nShReinfBranches,areaShReinfBranch,spacing):
    self.D2Section.shReinfZ.nShReinfBranches= nShReinfBranches # Número de ramas eficaces frente al cortante.
    self.D2Section.shReinfZ.areaShReinfBranch= areaShReinfBranch # Área de cada barra.
    self.D2Section.shReinfZ.shReinfSpacing= spacing

  def setShearReinfD1(self,nShReinfBranches,areaShReinfBranch,spacing):
    self.D1Section.shReinfZ.nShReinfBranches= nShReinfBranches # Número de ramas eficaces frente al cortante.
    self.D1Section.shReinfZ.areaShReinfBranch= areaShReinfBranch # Área de cada barra.
    self.D1Section.shReinfZ.shReinfSpacing= spacing

  def setMainReinf1neg(self,diam,area,spacing):
    self.D1Section.setMainReinfNeg(diam,area,spacing,self.basicCover+self.D2Section.negRebars.rebarsDiam)

  def setMainReinf1pos(self,diam,area,spacing):
    self.D1Section.setMainReinfPos(diam,area,spacing,self.basicCover+self.D2Section.posRebars.rebarsDiam)

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
