# -*- coding: utf-8 -*-
'''
Definición de una sección de hormigón armado con
armadura doblemente simétrica como las que se emplean
habitualmente en pilares.
'''

from materials.ehe import areaBarrasEHE
from materials.fiber_section import defSeccionHASimple

class ColumnMainReinforcement(object):
  ''' Main (longitudinal) rebars of a column.'''
  def __init__(self):
    self.recub= 0.030 # Recubrimiento.
    self.nBarrasAncho= 5 # Número de barras a colocar en el ancho (ambas caras).
    self.areaBarrasAncho= areaBarrasEHE.Fi12 # Área de las barras.

    self.nBarrasCanto= 2 # Número de barras a colocar en el canto (ambas caras).
    self.areaBarrasCanto= areaBarrasEHE.Fi10 # Área de las barras.


class RecordSeccionHAPilar(defSeccionHASimple.BasicRecordRCSection):
  '''
  Define las variables que se emplean para definir
   una sección de hormigón armado con armadura
   doblemente simétrica como las que se emplean
   habitualmente en pilares.
  '''

  def __init__(self):
    super(RecordSeccionHAPilar,self).__init__()
    self.mainBars= ColumnMainReinforcement()

  def defGeomSeccHAPilar(self,tipoDiag):
    '''
    Definición de una sección de hormigón armado con armadura
     doblemente simétrica como las que se emplean
     habitualmente en pilares.
     tipoDiag: Cadena de caracteres que será "k" si se emplea el diagrama característicoo "d" si se emplea el diagrama de cálculo.
    '''
    self.defDiagrams(mdlr,tipoDiag)

    geomSection= mdlr.getMaterialLoader.newSectionGeometry(self.nmbGeomSeccion())
    self.defConcreteRegion(geomSection)

    armaduras= sectionGeom.getReinfLayers
    p1= geom.Pos2d(-ancho/2+recub,-canto/2+recub) # Armadura inferior (cara -).
    p2= geom.Pos2d(ancho/2-recub,-canto/2+recub)

    self.armaduraInf= armaduras.newStraightReinfLayer(nmbDiagArmadura)
    self.armaduraInf.codigo= "anchoInf"
    self.armaduraInf.numReinfBars= nBarrasAncho
    self.armaduraInf.barArea= areaBarrasAncho
    self.armaduraInf.p1= geom.Pos2d(-ancho/2+recub,-canto/2+recub) # Armadura inferior (cara -).
    self.armaduraInf.p2= geom.Pos2d(ancho/2-recub,-canto/2+recub)

    self.armaduraSup= armaduras.newStraightReinfLayer(nmbDiagArmadura)
    self.armaduraSup.codigo= "anchoSup"
    self.armaduraSup.numReinfBars= nBarrasAncho
    self.armaduraSup.barArea= areaBarrasAncho
    self.armaduraSup.p1= geom.Pos2d(-ancho/2+recub,canto/2+recub) # Armadura superior (cara +).
    self.armaduraSup.p2= geom.Pos2d(ancho/2-recub,canto/2+recub)

    sepBarrasCanto= (canto-2*recub)/(nBarrasCanto+1)

    self.armaduraCIzq= armaduras.newStraightReinfLayer(nmbDiagArmadura)
    self.armaduraCIzq.codigo= "cantoIzda"
    self.armaduraCIzq.numReinfBars= nBarrasCanto
    self.armaduraCIzq.barArea= areaBarrasCanto
    self.armaduraCIzq.p1= geom.Pos2d(-ancho/2+recub,-canto/2+recub+sepBarrasCanto) # Armadura lateral izquierda.
    self.armaduraCIzq.p2= geom.Pos2d(-ancho/2+recub,canto/2-recub-sepBarrasCanto)

    self.armaduraCDer= armaduras.newStraightReinfLayer(nmbDiagArmadura)
    self.armaduraCDer.codigo= "cantoDcha"
    self.armaduraCDer.numReinfBars= nBarrasCanto
    self.armaduraCDer.barArea= areaBarrasCanto
    self.armaduraCDer.p1= geom.Pos2d(ancho/2-recub,-canto/2+recub+sepBarrasCanto) # Armadura lateral derecha.
    self.armaduraCDer.p2= geom.Pos2d(ancho/2-recub,canto/2-recub-sepBarrasCanto)

