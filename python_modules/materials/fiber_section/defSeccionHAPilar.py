# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"

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
    self.cover= 0.030 
    self.nRebarsWidth= 5 
    self.areaRebarWidth= areaBarrasEHE.Fi12 

    self.nRebarsDepth= 2 
    self.areaRebarDepth= areaBarrasEHE.Fi10 


class RecordRCColumnSection(defSeccionHASimple.BasicRecordRCSection):
  '''Definition of the variables that make up a reinforced concrete section 
  with reinforcement symmetric in both directions (as usual in columns)
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
    reinfDiagName:   name identifying the characteristic stress-strain diagram
                     of the reinforcing steel material
    shReinfZ:        record of type defSeccionHASimple.RecordShearReinforcement()
                      defining the shear reinforcement in Z direction
    shReinfY:        record of type defSeccionHASimple.RecordShearReinforcement()
                     defining the shear reinforcement in Y direction
    cover:          concrete clear cover of main reinforcement
    nRebarsWidth:   number of rebars in the width direction of the 
                    section (each face)
    areaRebarWidth: cross sectional area of each rebar in  width direction
    nRebarsDepth:   number of rebars in the depth direction of the 
                    section (each face)
    areaRebarDepth: cross sectional area of each rebar in the depth direction
  '''

  def __init__(self):
    super(RecordRCColumnSection,self).__init__()
    self.mainBars= ColumnMainReinforcement()

  def defGeomRCColumnSection(self,matDiagType):
    '''Returns a reinforced concrete section with reinforcement 
    symmetric in both directions (as usual in columns)
    Parameters:
     matDiagType: type of stress-strain diagram (="k" for characteristic diagram, 
                   ="d" for design diagram)
    '''
    self.defDiagrams(preprocessor,matDiagType)

    geomSection= preprocessor.getMaterialLoader.newSectionGeometry(self.gmSectionName())
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

