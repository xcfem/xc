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

class RecordArmaduraCortante(object):
  # Define una familia de armaduras de cortante
  nmbFamilia= "noName" # Nombre de la familia de armadura.
  nRamas= 0.0 # Número de ramas eficaces frente al cortante.
  areaRama= 0.0 # Área de cada barra.
  espaciamientoRamas= 0.2 # Separación entre planos.
  angAlphaRamas= math.pi/2.0 # Ángulo de las barras con el eje de la pieza.
  angThetaBielas= math.pi/4.0 # Ángulo de bielas de hormigón con el eje de la pieza.
  def __init__(self):
    self.nmbFamilia= "noName" # Nombre de la familia de armadura.
    self.nRamas= 0.0 # Número de ramas eficaces frente al cortante.
    self.areaRama= 0.0 # Área de cada barra.
    self.espaciamientoRamas= 0.2 # Separación entre planos.
    self.angAlphaRamas= math.pi/2.0 # Ángulo de las barras con el eje de la pieza.
    self.angThetaBielas= math.pi/4.0 # Ángulo de bielas de hormigón con el eje de la pieza.
    
  def getAs(self):
    return self.nRamas*self.areaRama/self.espaciamientoRamas

class RecordSeccionHASimple(object):
  '''
  Define las variables que se emplean para definir
     una sección de hormigón armado sencilla con una
     capa de armadura superior y otra inferior.
  '''
  nmbSeccion= "noName"
  descSeccion= "Texto que ayude a ubicar la sección."
  tipoHormigon= None
  nmbDiagHormigon= None
  canto= 0.25
  ancho= 0.25
  numDivIJ= 10
  numDivJK= 10

  # Armadura principal
  tipoArmadura= None
  nmbDiagArmadura= None # Nombre del material uniaxial.

  nBarrasNeg= 5 #Número de barras en la cara de negativos.
  areaBarrasNeg= areaBarrasEHE.Fi10 # Área de las barras.
  diamBarrasNeg= 10e-3 #Bar diameter.
  recubNeg= 0.030 # Recubrimiento cara de negativos.
  recubLatNeg= recubNeg # Recubrimiento lateral cara de negativos.

  nBarrasPos= 5 # Número de barras en la cara de positivos.
  areaBarrasPos= areaBarrasEHE.Fi10 # Área de las barras.
  diamBarrasPos= 10e-3 #Bar diameter.
  recubPos= 0.030 # Recubrimiento cara de positivos.
  recubLatPos= recubPos # Recubrimiento lateral cara de positivos.

  recubMin= 0.0 # Recubrimiento mínimo de la armadura longitudinal.

  # Armadura de cortante según z
  armCortanteZ= RecordArmaduraCortante()
  armCortanteZ.nmbFamilia= "Vz"

  # Armadura de cortante según y
  armCortanteY= RecordArmaduraCortante()
  armCortanteY.nmbFamilia= "Vy"

  def __init__(self):
    self.nmbSeccion= "noName"
    self.descSeccion= "Texto que ayude a ubicar la sección."
    self.tipoHormigon= None
    self.nmbDiagHormigon= None
    self.canto= 0.25
    self.ancho= 0.25
    self.numDivIJ= 10
    self.numDivJK= 10

    # Armadura principal
    self.tipoArmadura= None
    self.nmbDiagArmadura= None # Nombre del material uniaxial.

    self.nBarrasNeg= 5 #Número de barras en la cara de negativos.
    self.areaBarrasNeg= areaBarrasEHE.Fi10 # Área de las barras.
    self.recubNeg= 0.030 # Recubrimiento cara de negativos.
    self.recubLatNeg= self.recubNeg # Recubrimiento lateral cara de negativos.

    self.nBarrasPos= 5 # Número de barras en la cara de positivos.
    self.areaBarrasPos= areaBarrasEHE.Fi10 # Área de las barras.
    self.recubPos= 0.030 # Recubrimiento cara de positivos.
    self.recubLatPos= self.recubPos # Recubrimiento lateral cara de positivos.

    self.recubMin= 0.0 # Recubrimiento mínimo de la armadura longitudinal.

    # Armadura de cortante según z
    self.armCortanteZ= RecordArmaduraCortante()
    self.armCortanteZ.nmbFamilia= "Vz"

    # Armadura de cortante según y
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
    return self.nBarrasPos*self.areaBarrasPos
  def getYAsPos(self):
    return self.canto/2.0-self.recubPos
  def getAsNeg(self):
    return self.nBarrasNeg*self.areaBarrasNeg
  def getYAsNeg(self):
    return -self.canto/2.0+self.recubNeg
  def getAc(self):
    return self.ancho*self.canto
  def getI(self):
    return 1/12.0*self.ancho*self.canto**3

  def centraBarrasPos(self,sep):
    self.recubLatPos= (self.ancho-(self.nBarrasPos-1)*sep)/2.0
  def centraBarrasNeg(self,sep):
    self.recubLatNeg= (self.ancho-(self.nBarrasNeg-1)*sep)/2.0

  def defGeomSeccHASimple(self,mdlr,tipoDiag):
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
    if(self.nBarrasNeg>0):
      armaduraNeg= armaduras.newStraightReinfLayer(self.nmbDiagArmadura)
      armaduraNeg.codigo= "neg"
      armaduraNeg.numReinfBars= self.nBarrasNeg
      armaduraNeg.barArea= self.areaBarrasNeg
      y= self.getYAsNeg()
      armaduraNeg.p1= geom.Pos2d(-self.ancho/2+self.recubLatNeg,y) # Armadura inferior (cara -).
      armaduraNeg.p2= geom.Pos2d(self.ancho/2-self.recubLatNeg,y)

    if(self.nBarrasPos>0):
      armaduraPos= armaduras.newStraightReinfLayer(self.nmbDiagArmadura)
      armaduraPos.codigo= "pos"
      armaduraPos.numReinfBars= self.nBarrasPos
      armaduraPos.barArea= self.areaBarrasPos
      y= self.getYAsPos()
      armaduraPos.p1= geom.Pos2d(-self.ancho/2+self.recubLatPos,y) # Armadura superior (cara +).
      armaduraPos.p2= geom.Pos2d(self.ancho/2-self.recubLatPos,y)

    self.recubMin= min(self.recubLatNeg,min(self.recubLatPos,min(self.recubPos,self.recubNeg)))

  def defSeccionHASimple(self, mdlr,tipoDiag):
    '''
    Definición de una sección de hormigón armado sencilla
    con una capa de armadura superior y otra inferior.
    tipoDiag: Tipo del diagrama k: característico, d: cálculo.
    nmbRutinaDefGeom: Nombre de la rutina que define la geometría de la sección.
    '''
    JTorsion= parametrosSeccionRectangular.getJTorsion(self.ancho,self.canto)
    respT= typical_materials.defElasticMaterial(mdlr,self.nmbRespT(),self.tipoHormigon.Gcm()*JTorsion) # Respuesta de la sección a torsión.
    respVy= typical_materials.defElasticMaterial(mdlr,self.nmbRespVy(),5/6.0*self.ancho*self.canto*self.tipoHormigon.Gcm())# Respuesta de la sección a cortante según y.
    respVz= typical_materials.defElasticMaterial(mdlr,self.nmbRespVz(),5/6.0*self.ancho*self.canto*self.tipoHormigon.Gcm())# Respuesta de la sección a cortante según z.

    self.defGeomSeccHASimple(mdlr,tipoDiag)
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
    'Defines interaction diagram.'

    self.defGeomSeccHASimple(mdlr,tipoDiag)
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

  def getStressCalculator(self):
    Ec= self.tipoHormigon.Ecm()
    Es= self.tipoArmadura.Es
    return sc.StressCalc(self.ancho,self.canto,self.recubPos,self.recubNeg,self.getAsPos(),self.getAsNeg(),Ec,Es)

class RecordSeccionHALosa(object):
  '''
  Define las variables que se emplean para definir
     las secciones de hormigón armado de una losa sencilla con una
     capa de armadura superior y otra inferior.
  '''
  basicCover= 30e-3
  seccionT= None
  seccionL= None

  def __init__(self,nmb,desc,canto,concrete,steel,basicCover):
    self.basicCover= basicCover
    self.seccionT= RecordSeccionHASimple()
    self.seccionT.nmbSeccion= nmb + "T"
    self.seccionT.descSeccion= desc + ". T direction."
    self.seccionT.tipoHormigon= concrete
    self.seccionT.canto= canto
    self.seccionT.ancho= 1.0
    self.seccionT.tipoArmadura= steel

    self.seccionL= RecordSeccionHASimple()
    self.seccionL.nmbSeccion= nmb + "L"
    self.seccionL.descSeccion= desc + ". L direction."
    self.seccionL.tipoHormigon= concrete
    self.seccionL.canto= canto
    self.seccionL.ancho= 1.0
    self.seccionL.tipoArmadura= steel

  def setMainReinf2neg(self,diam,area,spacing):
    self.seccionT.diamBarrasNeg= diam
    self.seccionT.sepBarrasNeg= spacing
    nBarrasTeor= self.seccionT.ancho/self.seccionT.sepBarrasNeg
    self.seccionT.nBarrasNeg= int(math.floor(nBarrasTeor))
    factorReinf= nBarrasTeor/self.seccionT.nBarrasNeg
    self.seccionT.areaBarrasNeg= area*factorReinf
    self.seccionT.recubNeg= self.basicCover+self.seccionT.diamBarrasNeg/2.0
    self.seccionT.centraBarrasNeg(self.seccionT.sepBarrasNeg)

  def setMainReinf2pos(self,diam,area,spacing):
    self.seccionT.diamBarrasPos= diam
    self.seccionT.sepBarrasPos= spacing
    nBarrasTeor= self.seccionT.ancho/self.seccionT.sepBarrasPos
    self.seccionT.nBarrasPos= int(math.floor(nBarrasTeor))
    factorReinf= nBarrasTeor/self.seccionT.nBarrasPos
    self.seccionT.areaBarrasPos= area*factorReinf
    self.seccionT.recubPos= self.basicCover+self.seccionT.diamBarrasPos/2.0
    self.seccionT.centraBarrasPos(self.seccionT.sepBarrasPos)

  def setShearReinfT(self,nRamas,areaRama,spacing):
    self.seccionT.armCortanteZ.nRamas= nRamas # Número de ramas eficaces frente al cortante.
    self.seccionT.armCortanteZ.areaRama= areaRama # Área de cada barra.
    self.seccionT.armCortanteZ.espaciamientoRamas= spacing

  def setShearReinfL(self,nRamas,areaRama,spacing):
    self.seccionL.armCortanteZ.nRamas= nRamas # Número de ramas eficaces frente al cortante.
    self.seccionL.armCortanteZ.areaRama= areaRama # Área de cada barra.
    self.seccionL.armCortanteZ.espaciamientoRamas= spacing

  def setMainReinf1neg(self,diam,area,spacing):
    self.seccionL.diamBarrasNeg= diam
    self.seccionL.sepBarrasNeg= spacing
    nBarrasLeor= self.seccionL.ancho/self.seccionL.sepBarrasNeg
    self.seccionL.nBarrasNeg= int(math.floor(nBarrasLeor))
    factorReinf= nBarrasLeor/self.seccionL.nBarrasNeg
    self.seccionL.areaBarrasNeg= area*factorReinf
    self.seccionL.recubNeg= self.basicCover+self.seccionT.diamBarrasNeg+self.seccionL.diamBarrasNeg/2.0
    self.seccionL.centraBarrasNeg(self.seccionL.sepBarrasNeg)

  def setMainReinf1pos(self,diam,area,spacing):
    self.seccionL.diamBarrasPos= diam
    self.seccionL.sepBarrasPos= spacing
    nBarrasLeor= self.seccionL.ancho/self.seccionL.sepBarrasPos
    self.seccionL.nBarrasPos= int(math.floor(nBarrasLeor))
    factorReinf= nBarrasLeor/self.seccionL.nBarrasPos
    self.seccionL.areaBarrasPos= area*factorReinf
    self.seccionL.recubPos= self.basicCover+self.seccionT.diamBarrasPos+self.seccionL.diamBarrasPos/2.0
    self.seccionL.centraBarrasPos(self.seccionL.sepBarrasPos)
