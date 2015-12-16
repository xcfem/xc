# -*- coding: utf-8 -*-
'''
Definition of the variables that make up a reinforced concrete section 
with reinforcement symmetric in both directions (as usual in columns)
'''

from materials.ehe import areaBarrasEHE
from materials.fiber_section import defSeccionHASimple

class ColumnMainReinforcement(object):
  ''' Main (longitudinal) rebars of a column.
  Attributes:
  cover:          concrete clear cover of main reinforcement
  nRebarsWidth:   number of rebars in the width direction of the 
                  section (each face)
  areaRebarWidth: cross sectional area of each rebar in  width direction
  nRebarsDepth:   number of rebars in the depth direction of the 
                  section (each face)
  areaRebarDepth: cross sectional area of each rebar in  depth direction
  '''
  def __init__(self):
    self.cover= 0.030 # Recubrimiento.
    self.nRebarsWidth= 5 # Número de barras a colocar en el width (ambas caras).
    self.areaRebarWidth= areaBarrasEHE.Fi12 # Área de las barras.

    self.nRebarsDepth= 2 # Número de barras a colocar en el depth (ambas caras).
    self.areaRebarDepth= areaBarrasEHE.Fi10 # Área de las barras.


class RecordRCColumnSection(defSeccionHASimple.BasicRecordRCSection):
  '''Definition of the variables that make up a reinforced concrete section 
  with reinforcement symmetric in both directions (as usual in columns)
  Attributes:
  cover:          concrete clear cover of main reinforcement
  nRebarsWidth:   number of rebars in the width direction of the 
                  section (each face)
  areaRebarWidth: cross sectional area of each rebar in  width direction
  nRebarsDepth:   number of rebars in the depth direction of the 
                  section (each face)
  areaRebarDepth: cross sectional area of each rebar in  depth direction
  '''

  def __init__(self):
    super(RecordRCColumnSection,self).__init__()
    self.mainBars= ColumnMainReinforcement()

  def defGeomSeccHAPilar(self,tipoDiag):
    '''
    Definición de una sección de hormigón armado con armadura
     doblemente simétrica como las que se emplean
     habitualmente en pilares.
     tipoDiag: Cadena de caracteres que será "k" si se emplea el diagrama característicoo "d" si se emplea el diagrama de cálculo.
    '''
    self.defDiagrams(preprocessor,tipoDiag)

    geomSection= preprocessor.getMaterialLoader.newSectionGeometry(self.nmbGeomSeccion())
    self.defConcreteRegion(geomSection)

    armaduras= sectionGeom.getReinfLayers
    p1= geom.Pos2d(-width/2+cover,-depth/2+cover) # Armadura inferior (cara -).
    p2= geom.Pos2d(width/2-cover,-depth/2+cover)

    self.armaduraInf= armaduras.newStraightReinfLayer(reinfDiagName)
    self.armaduraInf.codigo= "widthInf"
    self.armaduraInf.numReinfBars= nRebarsWidth
    self.armaduraInf.barArea= areaRebarWidth
    self.armaduraInf.p1= geom.Pos2d(-width/2+cover,-depth/2+cover) # Armadura inferior (cara -).
    self.armaduraInf.p2= geom.Pos2d(width/2-cover,-depth/2+cover)

    self.armaduraSup= armaduras.newStraightReinfLayer(reinfDiagName)
    self.armaduraSup.codigo= "widthSup"
    self.armaduraSup.numReinfBars= nRebarsWidth
    self.armaduraSup.barArea= areaRebarWidth
    self.armaduraSup.p1= geom.Pos2d(-width/2+cover,depth/2+cover) # Armadura superior (cara +).
    self.armaduraSup.p2= geom.Pos2d(width/2-cover,depth/2+cover)

    rebarsSpacingCanto= (depth-2*cover)/(nRebarsDepth+1)

    self.armaduraCIzq= armaduras.newStraightReinfLayer(reinfDiagName)
    self.armaduraCIzq.codigo= "depthIzda"
    self.armaduraCIzq.numReinfBars= nRebarsDepth
    self.armaduraCIzq.barArea= areaRebarDepth
    self.armaduraCIzq.p1= geom.Pos2d(-width/2+cover,-depth/2+cover+rebarsSpacingCanto) # Armadura lateral izquierda.
    self.armaduraCIzq.p2= geom.Pos2d(-width/2+cover,depth/2-cover-rebarsSpacingCanto)

    self.armaduraCDer= armaduras.newStraightReinfLayer(reinfDiagName)
    self.armaduraCDer.codigo= "depthDcha"
    self.armaduraCDer.numReinfBars= nRebarsDepth
    self.armaduraCDer.barArea= areaRebarDepth
    self.armaduraCDer.p1= geom.Pos2d(width/2-cover,-depth/2+cover+rebarsSpacingCanto) # Armadura lateral derecha.
    self.armaduraCDer.p2= geom.Pos2d(width/2-cover,depth/2-cover-rebarsSpacingCanto)

