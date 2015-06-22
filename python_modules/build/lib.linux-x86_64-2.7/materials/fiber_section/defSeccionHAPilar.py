# -*- coding: utf-8 -*-
'''
Definición de una sección de hormigón armado con
armadura doblemente simétrica como las que se emplean
habitualmente en pilares.
'''

from materials.ehe import areaBarrasEHE
from materials.fiber_section import defSeccionHASimple

class RecordSeccionHAPilar(object):
  '''
  Define las variables que se emplean para definir
   una sección de hormigón armado con armadura
   doblemente simétrica como las que se emplean
   habitualmente en pilares.
  '''
  nmbSeccion= "noName"
  descSeccion= "Texto que ayude a ubicar la sección."
  nmbGeomSeccion= "noName"
  tipoHormigon= None 
  nmbDiagHormigon= None
  canto= 0.25
  ancho= 0.25
  numDivIJ= 10
  numDivJK= 10

  # Armadura principal
  tipoArmadura= None
  nmbDiagArmadura= None #Nombre del material uniaxial.

  recub= 0.030 # Recubrimiento.
  nBarrasAncho= 5 # Número de barras a colocar en el ancho (ambas caras).
  areaBarrasAncho= areaBarrasEHE.Fi12 # Área de las barras.

  nBarrasCanto= 2 # Número de barras a colocar en el canto (ambas caras).
  areaBarrasCanto= areaBarrasEHE.Fi10 # Área de las barras.

  # Armadura de cortante según z
  armCortanteZ= defSeccionHASimple.RecordArmaduraCortante()
  armCortanteZ.nmbFamilia= "Vz"

  # Armadura de cortante según y
  armCortanteY= defSeccionHASimple.RecordArmaduraCortante()
  armCortanteY.nmbFamilia= "Vy"


  def defGeomSeccHAPilar(self,tipoDiag):
    '''
    Definición de una sección de hormigón armado con armadura
     doblemente simétrica como las que se emplean
     habitualmente en pilares.
    tipoDiag: Cadena de caracteres que será "k" si se emplea el diagrama característicoo "d" si se emplea el diagrama de cálculo.
    '''
    if(tipoDiag=="d"):
      tipoHormigon.nmbDiagHormigon= nmbDiagD
      tipoArmadura.nmbDiagArmadura= nmbDiagD
    elif(tipoDiag=="k"):
      tipoHormigon.nmbDiagHormigon= nmbDiagK
      tipoArmadura.nmbDiagArmadura= nmbDiagK

    self.nmbGeomSeccion= "geom"+nmbSeccion
    geomSection= mdlr.getMaterialLoader.newSectionGeometry(nmbGeomSeccion)
    regiones= geomSection.getRegions
    rg= regiones.newQuadRegion(nmbDiagHormigon) # Hormigón
    rg.nDivIJ= numDivIJ
    rg.nDivJK= numDivJK
    rg.pMin= geom.Pos2d(-ancho/2,-canto/2)
    rg.pMax= geom.Pos2d(ancho/2,canto/2)
    armaduras= sectionGeom.getReinfLayers
    armaduraInf= armaduras.newStraightReinfLayer(nmbDiagArmadura)
    armaduraInf.codigo= "anchoInf"
    armaduraInf.numReinfBars= nBarrasAncho
    armaduraInf.barArea= areaBarrasAncho
    armaduraInf.p1= geom.Pos2d(-ancho/2+recub,-canto/2+recub) # Armadura inferior (cara -).
    armaduraInf.p2= geom.Pos2d(ancho/2-recub,-canto/2+recub)

    armaduraSup= armaduras.newStraightReinfLayer(nmbDiagArmadura)
    armaduraSup.codigo= "anchoSup"
    armaduraSup.numReinfBars= nBarrasAncho
    armaduraSup.barArea= areaBarrasAncho
    armaduraSup.p1= geom.Pos2d(-ancho/2+recub,canto/2+recub) # Armadura superior (cara +).
    armaduraSup.p2= geom.Pos2d(ancho/2-recub,canto/2+recub)

    sepBarrasCanto= (canto-2*recub)/(nBarrasCanto+1)

    armaduraCIzq= armaduras.newStraightReinfLayer(nmbDiagArmadura)
    armaduraCIzq.codigo= "cantoIzda"
    armaduraCIzq.numReinfBars= nBarrasCanto
    armaduraCIzq.barArea= areaBarrasCanto
    armaduraCIzq.p1= geom.Pos2d(-ancho/2+recub,-canto/2+recub+sepBarrasCanto) # Armadura lateral izquierda.
    armaduraCIzq.p2= geom.Pos2d(-ancho/2+recub,canto/2-recub-sepBarrasCanto)

    armaduraCDer= armaduras.newStraightReinfLayer(nmbDiagArmadura)
    armaduraCDer.codigo= "cantoDcha"
    armaduraCDer.numReinfBars= nBarrasCanto
    armaduraCDer.barArea= areaBarrasCanto
    armaduraCDer.p1= geom.Pos2d(ancho/2-recub,-canto/2+recub+sepBarrasCanto) # Armadura lateral derecha.
    armaduraCDer.p2= geom.Pos2d(ancho/2-recub,canto/2-recub-sepBarrasCanto)

