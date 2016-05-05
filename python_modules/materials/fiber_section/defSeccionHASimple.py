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
    nominalCover:    clear cover 
    cover:         effective cover (nominalCover+fi/2)
  '''
  def __init__(self,rebarsDiam=10e-3,areaRebar= areaBarrasEHE.Fi10,rebarsSpacing=0.2,width=1.0,nominalCover=0.03):
    self.rebarsDiam= rebarsDiam
    self.rebarsSpacing= rebarsSpacing
    nRebarsTeor= width/self.rebarsSpacing
    self.nRebars= int(math.floor(nRebarsTeor))
    self.areaRebar= areaRebar
    self.cover= nominalCover+self.rebarsDiam/2.0
    self.centerRebars(width)
  def setUp(self,nRebars= 5, rebarsDiam=10e-3,areaRebar= areaBarrasEHE.Fi10,width=1.0,cover=0.03):
    self.nRebars= nRebars
    self.rebarsDiam= rebarsDiam
    if(self.nRebars!=0.0):
      self.rebarsSpacing= width/self.nRebars
      self.centerRebars(width)
    else:
      self.rebarsSpacing= 100.0
    self.areaRebar= areaRebar
    self.cover= cover
    
  def getAs(self):
    '''returns the total cross-sectional area of reinforcing steel in the family
    '''
    return self.nRebars*self.areaRebar
  def centerRebars(self,width):
    '''center the row of rebars in the width of the section'''
    self.coverLat= (width-(self.nRebars-1)*self.rebarsSpacing)/2.0

  def defReinfLayer(self,reinforcement,code,nmbDiagram,p1,p2):
    '''Definition of a reinforcement layer in the fiber section model 
    between the 2d positions p1 and p2.
    '''
    if(self.nRebars>0):
      self.reinfLayer= reinforcement.newStraightReinfLayer(nmbDiagram)
      self.reinfLayer.codigo= code
      self.reinfLayer.numReinfBars= self.nRebars
      #print "reinforcement ", cod, " num. barras: ", self.reinfLayer.numReinfBars
      self.reinfLayer.barDiameter= self.rebarsDiam
      self.reinfLayer.barArea= self.areaRebar
      #print "reinforcement", cod, " bar area= ", self.reinfLayer.barArea*1e6, " mm2"
      #print "reinforcement", cod, " bar diam: ", self.rebarsDiam*1e3, " mm"
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
    fiberSectionRepr: fiber model of the section.
    reinfSteelType:  type of reinforcement steel
    reinfDiagName:   name identifying the characteristic stress-strain diagram
                     of the reinforcing steel material
    shReinfZ:        record of type.defRCSimpleSection.RecordShearReinforcement()
                      defining the shear reinforcement in Z direction
    shReinfY:        record of type.defRCSimpleSection.RecordShearReinforcement()
                     defining the shear reinforcement in Y direction
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
    ''' returns the name of the geometric section'''
    return "geom"+self.sectionName
  def respTName(self):
    ''' returns a name to identify the torsional response of the section'''
    return self.sectionName+"RespT"
  def respVyName(self):
    ''' returns a name to identify the shear Y response of the section'''
    return self.sectionName+"RespVy"
  def respVzName(self):
    ''' returns a name to identify the shear Z response of the section'''
    return self.sectionName+"RespVz"

  def getConcreteDiagram(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.concrDiagName)
  def getSteelDiagram(self,preprocessor):
    return preprocessor.getMaterialLoader.getMaterial(self.reinfDiagName)
  def getSteelEquivalenceCoefficient(self,preprocessor):
    tangHorm= self.getConcreteDiagram(preprocessor).getTangent()
    tangSteel= self.getSteelDiagram(preprocessor).getTangent()
    return tangSteel/tangHorm

  def defDiagrams(self,preprocessor,matDiagType):
    '''
    Stress-strain diagrams definition.
    '''
    self.diagType= matDiagType
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


class RecordRCSimpleSection(BasicRecordRCSection):
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
    fiberSectionRepr: fiber model of the section
    reinfSteelType:  type of reinforcement steel
    reinfDiagName:   name identifying the characteristic stress-strain diagram of
                     the reinforcing steel material
    shReinfZ:        record of type.defRCSimpleSection.RecordShearReinforcement()
                     defining the shear reinforcement in Z direction
    shReinfY:        record of type.defRCSimpleSection.RecordShearReinforcement() 
                     defining the shear reinforcement in Y direction
    coverMin:        minimum value of end or clear concrete cover of main bars from
                     both the positive and negative faces
    negatvRebarRows:       layers of main rebars in the local negative face of the section
    positvRebarRows:       layers of main rebars in the local positive face of the section
  '''
  def __init__(self):
    super(RecordRCSimpleSection,self).__init__()

    # Longitudinal reinforcement
    self.coverMin= 0.0 
    self.positvRebarRows= []  #list of MainReinfLayer data (positive face)
    self.negatvRebarRows= [] #list of MainReinfLayer data (negative face)
    self.posReinfLayers=[]  #list of xc.StraightReinfLayer created (positive face)
    self.negReinfLayers=[]  #list of xc.StraightReinfLayer created (negative face)

  def getAsPosRows(self):
    '''returns a list with the cross-sectional area of the rebars in each row of the positive face'''
    retval=[]
    for rbRow in self.positvRebarRows:
      retval.append(rbRow.getAs())
    return retval

  def getAsNegRows(self):
    '''returns a list with the cross-sectional area of the rebars in each row of the negative face'''
    retval=[]
    for rbRow in self.negatvRebarRows:
      retval.append(rbRow.getAs())
    return retval

  def getAsPos(self):
    '''returns the cross-sectional area of the rebars in the positive face'''
    return sum(self.getAsPosRows())

  def getPosRowsCGcover(self):
    '''returns the distance from the center of gravity of the positive rebars
    to the positive face of the section 
    '''
    retval=0
    for rbRow in self.positvRebarRows:
      retval+=rbRow.getAs()*rbRow.cover
    return retval/self.getAsPos()

  def getYAsPos(self):
    '''returns the local Y coordinate of the center of gravity of the rebars
       in the positive face
    '''
    return self.depth/2.0-self.getPosRowsCGcover()

  def getAsNeg(self):
    '''returns the cross-sectional area of the rebars in the negative face'''
    return sum(self.getAsNegRows())

  def getNegRowsCGcover(self):
    '''returns the distance from the center of gravity of the negative rebars
    to the negative face of the section 
    '''
    retval=0
    for rbRow in self.negatvRebarRows:
      retval+=rbRow.getAs()*rbRow.cover
    return retval/self.getAsNeg()

  def getYAsNeg(self):
    '''returns the local Y coordinate of the center of gravity of the rebars
       in the negative face
    '''
    return -self.depth/2.0+self.getNegRowsCGcover()

  def getAc(self):
    '''returns the cross-sectional area of the section'''
    return self.width*self.depth
  def getI(self):
    '''returns the second moment of area about the middle axis parallel to the width '''
    return 1/12.0*self.width*self.depth**3

  def getSPos(self):
    '''returns a list with the distance between bars for each row of bars in local positive face.'''
    retval=[]
    for rbRow in self.positvRebarRows:
      retval.append(rbRow.rebarsSpacing)
    return retval

  def getSNeg(self):
    '''returns a list with the distance between bars for each row of bars in local negative face.'''
    retval=[]
    for rbRow in self.negatvRebarRows:
      retval.append(rbRow.rebarsSpacing)
    return retval

  def getDiamPos(self):
    '''returns a list with the bar diameter for each row of bars in local positive face.'''
    retval=[]
    for rbRow in self.positvRebarRows:
      retval.append(rbRow.rebarsDiam)
    return retval

  def getDiamNeg(self):
    '''returns a list with the bar diameter for each row of bars in local negative face.'''
    retval=[]
    for rbRow in self.negatvRebarRows:
      retval.append(rbRow.rebarsDiam)
    return retval

  def getNBarPos(self):
    '''returns a list with the number of bars for each row of bars in local positive face.'''
    retval=[]
    for rbRow in self.positvRebarRows:
      retval.append(rbRow.nRebars)
    return retval

  def getNBarNeg(self):
    '''returns a list with the number of bars for each row of bars in local negative face.'''
    retval=[]
    for rbRow in self.negatvRebarRows:
      retval.append(rbRow.nRebars)
    return retval

  def getCoverPos(self):
    '''returns a list with the cover of bars for each row of bars in local positive face.'''
    retval=[]
    for rbRow in self.positvRebarRows:
      retval.append(rbRow.cover)
    return retval

  def getCoverNeg(self):
    '''returns a list with the cover of bars for each row of bars in local negative face.'''
    retval=[]
    for rbRow in self.negatvRebarRows:
      retval.append(rbRow.cover)
    return retval

  def getLatCoverPos(self):
    '''returns a list with the lateral cover of bars for each row of bars in local positive face.'''
    retval=[]
    for rbRow in self.positvRebarRows:
      retval.append(rbRow.coverLat)
    return retval

  def getLatCoverNeg(self):
    '''returns a list with the lateral cover of bars for each row of bars in local negative face.'''
    retval=[]
    for rbRow in self.negatvRebarRows:
      retval.append(rbRow.coverLat)
    return retval

  def centerRebarsPos(self):
    '''centers in the width of the section the rebars placed in the positive face''' 
    for rbRow in self.positvRebarRows:
      rbRow.centerRebars(self.width)

  def centerRebarsNeg(self):
    '''centers in the width of the section the rebars placed in the negative face''' 
    for rbRow in self.negatvRebarRows:
      rbRow.centerRebars(self.width)

  def defSectionGeometry(self,preprocessor,matDiagType):
    '''
    Definition of a reinforced concrete geometric section 
    with one row of rebars in the top face and another one in the bottom face
    Parameters:
      matDiagType: type of stress-strain diagram (="k" for characteristic diagram, 
                   ="d" for design diagram)
    '''
    self.defDiagrams(preprocessor,matDiagType)

    geomSection= preprocessor.getMaterialLoader.newSectionGeometry(self.gmSectionName())
    self.defConcreteRegion(geomSection)

    reinforcement= geomSection.getReinfLayers
    for rbRow in self.negatvRebarRows:
      y= -self.depth/2.0+rbRow.cover
      #print "y neg.= ", y, " m"
      p1= geom.Pos2d(-self.width/2+rbRow.coverLat,y)
      p2= geom.Pos2d(self.width/2-rbRow.coverLat,y)
      self.negReinfLayers.append(rbRow.defReinfLayer(reinforcement,"neg",self.reinfDiagName,p1,p2))

    for rbRow in self.positvRebarRows:
      y= self.depth/2.0-rbRow.cover
      p1= geom.Pos2d(-self.width/2+rbRow.coverLat,y)
      p2= geom.Pos2d(self.width/2-rbRow.coverLat,y)
      self.posReinfLayers.append(rbRow.defReinfLayer(reinforcement,"pos",self.reinfDiagName,p1,p2))

    self.coverMin= min(min(self.getCoverPos()),min(self.getCoverNeg()),min(self.getLatCoverPos()),min(self.getLatCoverNeg()))

  def getJTorsion(self):
    return parametrosSeccionRectangular.getJTorsion(self.width,self.depth)

  def getRespT(self,preprocessor,JTorsion):
    '''Material for modeling torsional response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.respTName(),self.concrType.Gcm()*JTorsion) # Respuesta de la sección a torsión.

  def getRespVy(self,preprocessor):
    '''Material for modeling z shear response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.respVyName(),5/6.0*self.width*self.depth*self.concrType.Gcm())

  def getRespVz(self,preprocessor):
    '''Material for modeling z shear response of section'''
    return typical_materials.defElasticMaterial(preprocessor,self.respVzName(),5/6.0*self.width*self.depth*self.concrType.Gcm())

  def defFiberSection(self,preprocessor):
    self.fs= preprocessor.getMaterialLoader.newMaterial("fiberSectionShear3d",self.sectionName)
    self.fiberSectionRepr= self.fs.getFiberSectionRepr()
    self.fiberSectionRepr.setGeomNamed(self.gmSectionName())
    self.fs.setupFibers()

    self.fs.setRespVyByName(self.respVyName())
    self.fs.setRespVzByName(self.respVzName())
    self.fs.setRespTByName(self.respTName())
    self.fs.setProp("datosSecc",self)

  def defRCSimpleSection(self, preprocessor,matDiagType):
    '''
    Definition of a reinforced concrete section with several
    top and bottom reinforcement layers.
    matDiagType: type of stress-strain diagram (="k" for characteristic diagrama, 
                 ="d" for design diagram)
     '''
    self.JTorsion= self.getJTorsion()
    self.respT= self.getRespT(preprocessor,self.JTorsion) # Respuesta de la sección a torsión.
    self.respVy= self.getRespVy(preprocessor)
    self.respVz= self.getRespVz(preprocessor)

    self.defSectionGeometry(preprocessor,matDiagType)
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
    return sc.StressCalc(self.width,self.depth,self.getPosRowsCGcover(),self.getNegRowsCGcover(),self.getAsPos(),self.getAsNeg(),Ec,Es)

class RecordRCSlabSection(object):
  '''This class is used to define the variables that make up a reinforced concrete slab 
  section with several reinforcement layers in the top and bottom faces
  Attributes:
    name:    basic name to form the RC sections in direction 1 (name+'1') 
             and direction 2(name+'1') 
    sectionDescr:    section description
    depth:           cross-section depth (width=1.0)
    concrType:       type of concrete (e.g. hormigonesEHE.HA25)     
    reinfSteelType:  type of reinforcement steel
    posMainRebarRowsD1: layers of main rebars in direction 1 in the local 
                        positive face of the section (list of MainReinfLayer)
    negMainRebarRowsD1: layers of main rebars in direction 1 in the local 
                        negative face of the section (list of MainReinfLayer)
    posMainRebarRowsD2: layers of main rebars in direction 2 in the local 
                        positive face of the section (list of MainReinfLayer)
    negMainRebarRowsD2: layers of main rebars in direction 2 in the local 
                        negative face of the section (list of MainReinfLayer)

  '''
  def __init__(self,name,sectionDescr,depth,concrType,reinfSteelType):
    self.name= name
    self.D2Section= RecordRCSimpleSection()
    self.D2Section.sectionName= name + "2"
    self.D2Section.sectionDescr= sectionDescr + ". 2 direction."
    self.D2Section.concrType= concrType
    self.D2Section.depth= depth
    self.D2Section.width= 1.0
    self.D2Section.reinfSteelType= reinfSteelType
    self.D2Section.positvRebarRows=[]
    self.D2Section.negatvRebarRows=[]
#    self.setPosMainRebarRowsD2(posMainRebarRowsD2)
#    self.setNegMainRebarRowsD2(negMainRebarRowsD2)

    self.D1Section= RecordRCSimpleSection()
    self.D1Section.sectionName= name + "1"
    self.D1Section.sectionDescr= sectionDescr + ". 1 direction."
    self.D1Section.concrType= concrType
    self.D1Section.depth= depth
    self.D1Section.width= 1.0
    self.D1Section.reinfSteelType= reinfSteelType
    self.D1Section.positvRebarRows=[]
    self.D1Section.negatvRebarRows=[]
#    self.setPosMainRebarRowsD1(posMainRebarRowsD1)
#    self.setNegMainRebarRowsD1(negMainRebarRowsD1)

  # def setPosMainRebarRowsD1(self,rebarLayerList):
  #   '''Assigns...'''
  #   self.D1Section.positvRebarRows= rebarLayerList
  # def setNegMainRebarRowsD1(self,rebarLayerList):
  #   '''Assigns...'''
  #   self.D1Section.negatvRebarRows= rebarLayerList
  # def setPosMainRebarRowsD2(self,rebarLayerList):
  #   '''Assigns...'''
  #   self.D2Section.positvRebarRows= rebarLayerList
  # def setNegMainRebarRowsD2(self,rebarLayerList):
  #   '''Assigns...'''
  #   self.D2Section.negatvRebarRows= rebarLayerList
     

  def setShearReinfD2(self,nShReinfBranches,areaShReinfBranch,spacing):
    self.D2Section.shReinfZ.nShReinfBranches= nShReinfBranches # Número de ramas eficaces frente al cortante.
    self.D2Section.shReinfZ.areaShReinfBranch= areaShReinfBranch # Área de cada barra.
    self.D2Section.shReinfZ.shReinfSpacing= spacing

  def setShearReinfD1(self,nShReinfBranches,areaShReinfBranch,spacing):
    self.D1Section.shReinfZ.nShReinfBranches= nShReinfBranches # Número de ramas eficaces frente al cortante.
    self.D1Section.shReinfZ.areaShReinfBranch= areaShReinfBranch # Área de cada barra.
    self.D1Section.shReinfZ.shReinfSpacing= spacing

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
    '''get steel area.
    code='As1+' for direction 1, positive face
    code='As1-' for direction 1, negative face
    code='As2+' for direction 2, positive face
    code='As2-' for direction 2, negative face
    '''
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

  def getS1pos(self):
    '''list of distances between bars of rows the in local positive face direction 1.'''
    return self.D1Section.getSPos()
  def getS1neg(self):
    '''list of distances between bars of rows  in the local negative face direction 1.'''
    return self.D1Section.getSNeg()
  def getS2pos(self):
    '''list of distances between bars of rows  in the local positive face direction 2.'''
    return self.D2Section.getSPos()
  def getS2neg(self):
    '''list of distances between bars of rows  in the local negative face direction 2.'''
    return self.D2Section.getSNeg()
  def getS(self,code):
    '''list of distances between bars
    code='s1+' for direction 1, positive face
    code='s1-' for direction 1, negative face
    code='s2+' for direction 2, positive face
    code='s2-' for direction 2, negative face
    '''
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
    '''list of bar diameter in rows of the local negative face direction 1.'''
    return self.D1Section.getDiamNeg()
  def getDiam1pos(self):
    '''list of bar diameter in rows of the local positive face direction 1.'''
    return self.D1Section.getDiamPos()
  def getDiam2neg(self):
    '''list of bar diameter in rows of the local negative face direction 2.'''
    return self.D2Section.getDiamNeg()
  def getDiam2pos(self):
    '''list of bar diameter in rows of the local positive face direction 2.'''
    return self.D2Section.getDiamPos()
  def getDiam(self,code):
    '''list of bar diameter.'''
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

  def getNBar1pos(self):
    '''list of number of bars in rows of the local positive face direction 1.'''
    return self.D1Section.getNBarPos()
  def getNBar1neg(self):
    '''list of number of bars in rows of the local negative face direction 1.'''
    return self.D1Section.getNBarNeg()
  def getNBar2pos(self):
    '''list of number of bars in rows of the local positive face direction 2.'''
    return self.D2Section.getNBarPos()
  def getNBar2neg(self):
    '''list of number of bars in rows of the local negative face direction 2.'''
    return self.D2Section.getNBarNeg()
  def getNBar(self,code):
    '''list of number of bars.'''
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
